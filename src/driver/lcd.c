#include <hardware/pio.h>
#include <hardware/dma.h>
#include "hardware/clocks.h"
#include "lcd.h"
#include "lcd.pio.h"

// ****                  LC79400+LC79430 LCD PIO+DMA Driver                  ****
// ****                       By mateuszdrwal (SA6DRW)                       ****

// This driver uses 1 PIO (with a 6 instruction program) and 2 DMA channels to
// drive the LCD completely in the background with no supervision or
// interraction by any core. Simply write to the framebuffer to display graphics
// on the display.

// Arguments:

// uint32_t *buf[] - The framebuffer with length 2400 ((320/8)*240 bytes)
// char pin - The first GPIO pin (FLM). All 7 pins must be consecutive
// PIO pio - The PIO to use.

// Pin mapping:

// pin+0 - FLM
// pin+1 - LOAD
// pin+2 - CP
// pin+3 - D1
// pin+4 - D2
// pin+5 - D3
// pin+6 - D4

// LCD theory of operation:

// The LCD has 3 control pins and a 4 pin parallel data bus. The control pins
// are: FLM - First Line Marker, LOAD - Loads next row, CP - Data clock. On the
// falling edge of CP the 4 bits on the data bus are pushed onto the display.
// Pulsing LOAD indicates we are starting to draw the next line. Additionaly,
// FLM should be held high for the entirety of the first line, including the
// LOAD pulse. Specifically, drawing a frame ammounts to:

// 1. Pull FLM high
// 2. Push a row of data onto the screen by pulsing CP 320/4=80 times while the
//    first rows pixel data is on the bus.
// 3. Pulse LOAD
// 4. Pull FLM low
// 5. Repeat steps 2 and 3 240-1=239 more times, drawing the rest of the rows
// 6. Pulse LOAD one more time in order to prevent overdrawing the last row

// The next frame should be drawn as soon as possible, since the LCD does not
// refresh itself, otherwise the image will blink or fade. I found a refresh
// rate of around 130Hz works well.

// Driver theory of operation:

// The PIO program in lcd.pio pulls framebuffer data from the FIFO and outputs
// it to the LCD in the correct way described above. It includes an 80 iteration
// loop, and since loading this value in code is hard (the set instruction maxes
// out at setting a 5-bit immediate value) it is injected into the state machine
// on init into the Y register. The PIO program only handles outputing display
// rows and clearing FLM at the end of each, so FLM has to be set high from
// outside at the beginning of each frame.

// This leaves two things to be done outside of the PIO, namely streaming the
// framebuffer and setting the FLM pin high at the start of a frame. Both of
// these are done by two DMA peripherals. One DMA is the "worker" DMA which
// actually does things, while the other DMA is the "control" DMA which
// continously reconfigures the first DMA to do its different jobs.

// A DMA is configured using 4 registers, stored in the control_block struct.
// The control_blocks[] array stores a list of control bocks for the worked DMA
// to "execute". These are described in more detail in comments below, but
// roughly the chain of events is:

// 1. The worker DMA sets the FLM pin high and chains to the control DMA
// 2. The control DMA reconfigures the worker DMA to perform its next task,
//    starting it in the process
// 3. The worker DMA streams the framebuffer to the PIO and chains to the
//    control DMA
// 4. The control DMA reconfigures the worker DMA to perform its next task,
//    starting it in the process
// 5. The worker DMA implements a delay to wait for the PIO to drain its FIFO
//    onto the screen, and chains to the control DMA
// 6. The control DMA reconfigures the worker DMA to perform its next task,
//    starting it in the process
// 7. The worker DMA reconfigures the control DMA to load the configuration for
//    step 1 next, starting it in the process
// 6. The control DMA reconfigures the worker DMA to perform step 1, starting it
//    in the process and thus restarting the loop

struct control_block {
    uint32_t *source;
    volatile uint32_t *dest;
    uint32_t count;
    dma_channel_config ctrl;
};

static struct control_block control_blocks[4];
static struct control_block dma_control_control_block;

static uint32_t pio_flm_instruction;

void lcd_init(uint32_t *buf, char pin, PIO pio) {

    uint offset = pio_add_program(pio, &lcd_program);
    uint sm = pio_claim_unused_sm(pio, true);

    pio_sm_config config = lcd_program_get_default_config(offset);

    // init pin mappings
    // pin 1: FLM - set to set
    sm_config_set_set_pins(&config, pin, 1);
    // pin 2: LOAD - set to side-set
    // pin 3: CP - set to side-set
    sm_config_set_sideset_pins(&config, pin+1);
    // pin 4: D1 - set to out
    // pin 5: D2 - set to out
    // pin 6: D3 - set to out
    // pin 7: D4 - set to out
    sm_config_set_out_pins(&config, pin+3, 4);

    // init pins, there probably exists a better way?
    pio_gpio_init(pio, pin);
    pio_gpio_init(pio, pin+1);
    pio_gpio_init(pio, pin+2);
    pio_gpio_init(pio, pin+3);
    pio_gpio_init(pio, pin+4);
    pio_gpio_init(pio, pin+5);
    pio_gpio_init(pio, pin+6);
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 7, true);

    // pio frequency, this sets the data output speed. 20MHz seems to work well
    int pio_freq = 20000000;
    sm_config_set_clkdiv(&config, clock_get_hz(clk_sys) / pio_freq);

    // configure autopull
    sm_config_set_out_shift(&config, false, true, 8);

    pio_sm_init(pio, sm, offset, &config);

    // inject the value 79 into the Y register
    pio->sm[sm].instr = pio_encode_out(pio_y, 32);
    pio_sm_put_blocking(pio, sm, 79);

    // start the sm, executing the above instruction first
    pio_sm_set_enabled(pio, sm, true);

    int dma_worker = dma_claim_unused_channel(true);
    int dma_control = dma_claim_unused_channel(true);

    // dma_control: This control block writes control_blocks[] control blocks to
    // dma_worker, restarting it with the next block
    dma_control_control_block.source = (uint32_t*) &control_blocks[0].source;
    dma_control_control_block.dest = &dma_hw->ch[dma_worker].read_addr;
    dma_control_control_block.count = 4;
    dma_control_control_block.ctrl = dma_channel_get_default_config(dma_control);
    channel_config_set_transfer_data_size(&dma_control_control_block.ctrl, DMA_SIZE_32);
    channel_config_set_read_increment(&dma_control_control_block.ctrl, true);
    channel_config_set_write_increment(&dma_control_control_block.ctrl, true);
    channel_config_set_ring(&dma_control_control_block.ctrl, true, 4);

    // counter to simplify writing control blocks
    int i = 0;

    // Worker control block: Make PIO set FLM pin high. This injects a set
    // instruction to be executed immediately into the PIO. At this point the
    // PIO should always be stalled at the out instruction, meaning CP is high.
    // We need to add a side-set to the injected instruction in order to prevent
    // toggling CP and loading 4 bits of garbage onto the display.
    pio_flm_instruction = pio_encode_set(pio_pins, 1) | pio_encode_sideset(2, 0b10);
    control_blocks[i].source = &pio_flm_instruction;
    control_blocks[i].dest = &pio->sm[sm].instr;
    control_blocks[i].count = 1;
    control_blocks[i].ctrl = dma_channel_get_default_config(dma_worker);
    channel_config_set_transfer_data_size(&control_blocks[i].ctrl, DMA_SIZE_32);
    channel_config_set_chain_to(&control_blocks[i].ctrl, dma_control);
    i += 1;

    // Worker control block: Streams framebuffer to pio
    control_blocks[i].source = buf;
    control_blocks[i].dest = &pio->txf[sm];
    // +1 below to write an extra row which toggles the LOAD signal once more.
    // This is necessary so that the last row on the display does not get
    // overdriven and displays all white. The DMA will read 240 bytes of junk
    // outside of the framebuffer, but this data doesn't actually go anywhere
    // and reading from those addresses on the rp2040 should be fine
    control_blocks[i].count = 320/8*(240+1);
    control_blocks[i].ctrl = dma_channel_get_default_config(dma_worker);
    channel_config_set_transfer_data_size(&control_blocks[i].ctrl, DMA_SIZE_8);
    channel_config_set_read_increment(&control_blocks[i].ctrl, true);
    channel_config_set_write_increment(&control_blocks[i].ctrl, false);
    channel_config_set_dreq(&control_blocks[i].ctrl, pio_get_dreq(pio, sm, true));
    channel_config_set_chain_to(&control_blocks[i].ctrl, dma_control);
    i += 1;

    // Worker control block: nop. This is to wait for the TX FIFO and OSR in the
    // PIO to get drained, so all data is transfered to the display. Reads and
    // writes to the same address contoniously to nop.
    control_blocks[i].source = &pio_flm_instruction;
    control_blocks[i].dest = &pio_flm_instruction;
    control_blocks[i].count = clock_get_hz(clk_sys) / pio_freq * 100;
    control_blocks[i].ctrl = dma_channel_get_default_config(dma_worker);
    channel_config_set_transfer_data_size(&control_blocks[i].ctrl, DMA_SIZE_32);
    channel_config_set_read_increment(&control_blocks[i].ctrl, false);
    channel_config_set_write_increment(&control_blocks[i].ctrl, false);
    channel_config_set_chain_to(&control_blocks[i].ctrl, dma_control);
    i += 1;

    // Worker control block: Reset dma_control with the original configuration,
    // which makes it load control_blocks[0] next and thus restarts the loop.
    // This is the only dma_worker control block we don't want to chain, since
    // we are writing to a dma_control trigger register.
    control_blocks[i].source = (uint32_t*) &dma_control_control_block.source;
    control_blocks[i].dest = &dma_hw->ch[dma_control].read_addr;
    control_blocks[i].count = 4;
    control_blocks[i].ctrl = dma_channel_get_default_config(dma_worker);
    channel_config_set_transfer_data_size(&control_blocks[i].ctrl, DMA_SIZE_32);
    channel_config_set_read_increment(&control_blocks[i].ctrl, true);
    channel_config_set_write_increment(&control_blocks[i].ctrl, true);

    // Start the main loop by configuring and triggering dma_control
    dma_channel_configure(
        dma_control, 
        &dma_control_control_block.ctrl,
        dma_control_control_block.dest,
        dma_control_control_block.source,
        dma_control_control_block.count,
        true
    );
}
