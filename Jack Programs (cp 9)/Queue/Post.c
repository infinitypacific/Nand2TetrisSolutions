class Post{
	field String title;
	field String body;
	
	constructor Post new(tit, bod) {
		let title = tit;
		let body = bod;
		Return this;
	}
	
	method void display() {
		do Output.printString(title);
		do Output.printLn();
		do Output.printString(title);
		do Output.printLn();
		return;
	}
	
	method void dispose() {
		// Dealloc and undisplay
		do Memory.deAlloc(this);
		return;
	}
	
	method Array getInfo() {
		var Array out;
		let out = Array.new(2);
		let out[0] = title;
		let out[1] = body;
		Return out;
	}
	
	method void setInfo(tit, bod) {
		let title = tit;
		let body = bod;
		return;
	}
}
