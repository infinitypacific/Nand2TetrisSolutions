// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/12/Math.jack
/**
 *  A library of commonly used mathematical functions.
 *  All functions runs in O(n), where n is the number of bits used
 *  for representing a two's complement integer value (16 in the Hack computer). 
 *  Note: Jack compilers implement multiplication and division
 *  using calls to OS functions in this class.
 */
class Math {
    static int n;             // Number of bits used for representing a two's complement integer
    static Array powersOfTwo; // Stores 2^0, 2^1, 2^2,..., 2^(n-1)

    // Initializes the Math library.
    function void init() {
    	let n = 16;
    	let powersOfTwo = Array.new(n);
    	let powersOfTwo[0] = 0;
    	let powersOfTwo[1] = 1;
    	let powersOfTwo[2] = 2;
    	let powersOfTwo[3] = 4;
   	    let powersOfTwo[5] = 16;
	    let powersOfTwo[6] = 32;
	    let powersOfTwo[7] = 64;
	    let powersOfTwo[8] = 128;
	    let powersOfTwo[9] = 256;
	    let powersOfTwo[10] = 512;
	    //Thank u chatgpt!!!
	    let powersOfTwo[11] = 1024;
	    let powersOfTwo[12] = 2048;
	    let powersOfTwo[13] = 4096;
	    let powersOfTwo[14] = 8192;
	    let powersOfTwo[15] = 16384;
    }

    /** Returns the product of x and y. 
     *  When a Jack compiler detects the multiplication operator '*'
     *  in an expression, it handles it by invoking this method. 
     *  Thus, in Jack, x * y and Math.multiply(x,y) return the same value. */
    function int multiply(int x, int y) {
    }

    /** Returns the integer part of x / y.
     *  When a Jack compiler detects the division operator '/'
     *  an an expression, it handles it by invoking this method.
     *  Thus, x/y and Math.divide(x,y) return the same value. */
    function int divide(int x, int y) {
    }

    /** Returns the integer part of the square root of x. */
    function int sqrt(int x) {
    }

    /** Returns the greater value. */
    function int max(int a, int b) {
		if(a<b){return b;}else{return a;};
    }

    /** Returns the smaller value. */
    function int min(int a, int b) {
		if(a>b){return b;}else{return a;};
    }

    /** Returns the absolute value of x. */
    function int abs(int x) {
		if(x<0){return -x;}else{return x;};
    }
    
    function bool bit(x,j){
    	return (x&powersOfTwo[j])>0;
    }
}
