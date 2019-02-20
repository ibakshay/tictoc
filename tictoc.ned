
import ned.DatarateChannel;

simple Tic
{
    parameters:
        @display("i=device/pc_s");
        volatile double delayTimeTic @unit(s);
        int counter = default(5);
    gates:
         input in;
        output out;
}

simple Toc
{
    parameters:
          
          @display("i=device/antennatower_l");
        volatile double delayTimeToc @unit(s);
    gates:
        input in;
        output out;
}

network TicToc 
{
    types:
        channel Channel extends ned.DatarateChannel {
            datarate = 100 Mbps;
            delay = 2000 ms;
        }
    submodules:
        tic: Tic;
        toc: Toc;
    connections:
         tic.out --> Channel --> toc.in;
        tic.in <-- Channel <-- toc.out;
}
