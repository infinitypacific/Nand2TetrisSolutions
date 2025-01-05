// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/12/Keyboard.jack
/**
 * A library for handling user input from the keyboard.
 */
class Keyboard {

    /** Initializes the keyboard. */
    function void init() {
    } 

    /**
     * Returns the character of the currently pressed key on the keyboard;
     * if no key is currently pressed, returns 0.
     *
     * Recognizes all ASCII characters, as well as the following keys:
     * new line = 128 = String.newline()
     * backspace = 129 = String.backspace()
     * left arrow = 130
     * up arrow = 131
     * right arrow = 132
     * down arrow = 133
     * home = 134
     * End = 135
     * page up = 136
     * page down = 137
     * insert = 138
     * delete = 139
     * ESC = 140
     * F1 - F12 = 141 - 152
     */
    function char keyPressed() {
    	return Memory.peek(24576);
    }

    /**	Waits until a key is pressed on the keyboard and released,
     *  then echoes the key to the screen, and returns the character 
     *  of the pressed key. */
    function char readChar() {
    	var char k;
    	while(k=0){
    		let k = Memory.peek(24576);//Optimized lol
    	}
    	Output.printChar(k);
    	while(~(Memory.peek(24576)=0)){}
    	return k;
    }

    /**	Displays the message on the screen, reads from the keyboard the entered
     *  text until a newline character is detected, echoes the text to the screen,
     *  and returns its value. Also handles user backspaces. */
    function String readLine(String message) {
    	var String inp;
    	var bool run;
    	var char key;
    	let inp = String.new(255);
    	let run = true;
    	do Output.printString(message);
    	while(run){
    		let key = Keyboard.readChar();
	    	if(key=128){
	    		let run = false;
	    	}else{
	    		if(key=129){
	    			do inp.eraseLastChar();
	    			//todo remove
	    		}else{
	    			do inp.appendChar(key)
	    		}
	    	}
    	}
    	return inp;
    }   

    /** Displays the message on the screen, reads from the keyboard the entered
     *  text until a newline character is detected, echoes the text to the screen,
     *  and returns its integer value (until the first non-digit character in the
     *  entered text is detected). Also handles user backspaces. */
    function int readInt(String message) {
    	var String inp;
    	let inp = Keyboard.readLine(message);
    	return inp.intValue();
    }
}
