// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/12/Sys.jack

/**
 * A library that supports various program execution services.
 */
class Sys {

    /** Performs all the initializations required by the OS. */
    function void init() {
    	Memory.init();
    	Keyboard.init();
    	Screen.init();
    	Output.init();
    	Math.init();
    	Main.main();
    	return;
    }

    /** Halts the program execution. */
    function void halt() {
    	while(true){};
    	return;
    }

    /** Waits approximately duration milliseconds and returns.  */
    function void wait(int duration) {
    	var int i;
    	while(duration > 0){
    		let i = 1000;
    		while(i > 0){
    			let i = i-1;
    		}
    		let duration = duration-1;
    	}
    	return;
    }

    /** Displays the given error code in the form "ERR<errorCode>",
     *  and halts the program's execution. */
    function void error(int errorCode) {
    	Output.moveCursor(0,0);
    	Output.printString("ERR");
    	Output.printInt(errorCode);
    	Sys.halt();
    	return;
    }
}
