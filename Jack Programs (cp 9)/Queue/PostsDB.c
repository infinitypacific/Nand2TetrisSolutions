/** Error codes:
0: Success
1: Warning
2: Fatal
*/
class PostDB {
	field List Posts;
	field int voffset;
	field int hoffset;
	field int totalLength;
	field int pageLength;
	field int cursor;
	field int prefix;
	field char bullet;
	
	constructor PostDB new(int hoff, int vhoff,int len, int pglen) {
		Return this;
	}
	
	method void swi(int a, int b) {
		
	}
	
	method void add(String title, String body) {
		var Post n;
		let n = Post.new(title, body);
		if(totalLength >= Posts.getLen()) {
			Posts.setLen(Posts.getLen() + pageLength);
			Posts.set(totalLength, n);
		}
		else {
			Posts.set(totalLength, n);
		}
		let totalLength = totalLength + 1;
		return;
	}
	
	method void rem() {
		var int i;
		do Posts.get(0).dispose();
		let i = 1;
		while (i < Posts.getLen()) {
			do Posts.set(i - 1, Posts.get(i));
		}
		let totalLength = totalLength - 1;
		return;
	}
	
	method int set() {
	
	}
	
	method int del() {
	
	}
	
	method int mov() {
	
	}
	
	function void underLine(int l) {
		do Screen.drawLine(0,(l * 11) - 1,512,(l * 11) - 1);
		return;
	}
	
	method int drawPosts() {
		
	}
	
	method void printStats() {
	
	}
	
	method int up(boolean page) {
	
	} 
	
	method int dwn(boolean page) {
	
	}
	
	method void setBullet() {
	
	}
	
	method int setPrefix() {
	
	}
	
	method int setOffset() {
	
	}
	
	method int setPage() {
	
	}
	
	method void setCursor() {
		
	}
	
	method void setPrompt() {
		
	}
	
	method Post getPost() {
	
	}
	
	method Array getStats() {
	
	}
}
