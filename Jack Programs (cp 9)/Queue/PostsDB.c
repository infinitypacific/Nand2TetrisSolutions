/** Error codes:
0: Success
1: Warning
2: Fatal
*/
// replace getlen by keeping len here
// if List doesnt work, use array and resize it
// Turn post into arrays
class PostDB {
	field List Posts;
	field int voffset;
	field int hoffset;
	field int totalLength;
	field int pageLength;
	field int cursor;
	field int prefix;
	field int listLen;
	field char bullet;
	
	constructor PostDB new(int hoff, int voff,int len, int pglen) {
		let Posts = List.new(len);
		let voffset = voff;
		let hoffset = hoff;
		let totalLength = 0;
		let pageLength = pglen;
		let cursor = 0;
		let prefix = 1;
		let listLen = len;
		let bullet = "*;
		Return this;
	}
	
	method void swi(int a, int b) {
		var Array Oa;
		let Oa = Posts.get(a);
		do Posts.set(a, Posts.get(b));
		do Posts.set(b, Oa);
		return;
	}
	
	method void add(String title, String body) {
		var Array n;
		let n = Array.new(2);
		let n[0] = title;
		let n[1] = body;
		if((totalLength + 1) > listLen) {
			let listLen = listLen + pageLength;
			do Posts.setLen(listLen);
		}
		do Posts.set(totalLength, n);
		let totalLength = totalLength + 1;
		return;
	}
	
	method int rem() {
		if (totalLength = 0) {
			Return 2;
		}
		var int i;
		do Posts.get(0).dispose();
		let i = 1;
		while (i < listLen) {
			do Posts.set(i - 1, Posts.get(i));
			let i = i + 1;
		}
		let totalLength = totalLength - 1;
		Return 0;
	}
	
	method int set(int i, String tit, String bod) {
		if ((i + 1) > totalLength) {
			Return 2;
		}
		var Array n;
		let n = Posts.get(i);
		let n[0] = tit;
		let n[1] = bod;
		Return 0;
	}
	
	method int del(int i) {
		if ((i + 1) > totalLength) {
			Return 2;
		}
		do Posts.get(i).dispose();
		var int ii;
		let ii = i + 1;
		while (ii < totalLength) {
			do Posts.set(ii - 1, Posts.get(ii));
			let ii = ii + 1;
		}
		let totalLength = totalLength - 1;
		Return 0;
	}
	
	method int ins(int i, String tit, String bod) {
		if ((i + 1) > totalLength) {
			Return 2;
		}
		var Array n;
		var int ii;
		let n = Array.new(2);
		let n[0] = tit;
		let n[1] = bod;
		let ii = i;
		while (ii < totalLength) {
			if ((ii + 2) > listLen) {
				let listLen = listLen + pageLength;
				do Posts.setLen(listen);
			}
			do Posts.set(ii + 1,Posts.get(ii));
			let ii = ii + 1;
		}
		do Posts.set(i,n);
		let totalLength = totalLength + 1;
		Return 0;
	}
	
	method int mov(int op, int np) {
		if ((op = np) | ((op + 1) > totalLength) | ((np + 1) > totalLength)) {
			Return 2;
		}
		var int opv,i;
		let opv = Posts.get(op);
		if (op > np) {
			// Store op, Push all posts from np to op-1 down
			let i = np;
			while (i < op) {
				do Posts.set(i + 1,Posts.get(i));
				let i = i + 1;
			}
			
		}
		else {
			// Store op, Push all posts from op+1 to np up
			let i = op + 1;
			while ((i - 1) < np) {
				do Posts.set(i - 1,Posts.get(i));
				let i = i + 1;
			}
		}
		do Posts.set(np,opv);
		Return 0;
	}
	
	function void underLine(int l) {
		var int rl;
		let rl = ((l + 1) * 11) - 1;
		do Screen.drawLine((hoffset * 8),rl,512,rl;
		return;
	}
	
	method void drawPosts() {
		var int i, target, l;
		let l = voffset;
		let i = cursor;
		let target = (cursor + pageLength) - 1;
		//Clamp target
		if ((target + 1) > totalLength) {
			let target = totalLength - 1;
		}
		while (i < target) {
			var Array info;
			let info = Posts.get(i);
			let i = i + 1;
			do Output.moveCursor(l,hoffset);
			if (prefix = 1) {
				do Output.printInt(i);
				do Output.printChar(20);
			}
			else {
				if (prefix = 2) {
					do Output.printChar(bullet);
					do Output.printChar(20);
				}
			}
			do Output.printString(info[0]);
			let l = l + 1;
			do Output.moveCursor(l, hoffset);
			if (prefix > 0) {
				do Output.printChar(20);
				do Output.printChar(20);
			}
			do Output.printString(info[1]);
			do PostsDB.underLine(l);
			let l = l + 1;
		}
		return;
	}
	
	method void printStats() {
		do Output.printString("Stats Total: ");
		do Output.printInt(totalLength);
		do Output.printString(" Page: ");
		do Output.printInt((totalLength / pageLength) + 1);
		do Output.printString(" Cursor: ");
		do Output.printInt(cursor + 1);
		return;
	}
	
	method int up(boolean page) {
		if (cursor = 0) {
			Return 2;
		}
		if (page) {
			let cursor = cursor - pageLength;
			if (cursor < 0) {
				let cursor = 0;
			}
		}
		else {
			let cursor = cursor - 1;
		}
		Return 0;
	} 
	
	method int dwn(boolean page) {
		if (cursor = (totalLength - 1)) {
			Return 2;
		}
		if (page) {
			let cursor = cursor + pageLength;
			if (cursor > (totalLength - 1)) {
				let cursor = totalLength - 1;
			}
		}
		else {
			let cursor = cursor + 1;
		}
		Return 0;
	}
	
	method void setBullet(char nbul) {
		let bullet = nbul;
		return;
	}
	
	method int setPrefix(int o) {
		if ((o > 2) | (o < 0)) {
			Return 2;
		}
		let prefix = o;
		Return 0;
	}
	
	method void setOffset(hoff,voff) {
		let hoffset = hoff;
		let voffset = voff;
		return;
	}
	
	method void setPage(int pgl) {
		let pageLength = pgl;
		return;
	}
	
	method int setCursor(int n) {
		if((n < 0) | (n > (totalLength - 1))) {
			Return 2;
		}
		let cursor = n;
		Return 0;
	}
	
	method int getPost(int i) {
		if((i < 0) | (i > (totalLength - 1))) {
			Return 2;
		}
		Return Posts.get(i);
	}
	
	method Array getPosts() {
		var Array opost;
		var int target, i, ni;
		let opost = Array.new(pageLength);
		let i = cursor;
		let target = pageLength + cursor;
		//keep opost always pagelength
		let ni = 0;
		if (target > (totalLength - 1)) {
			let target = totalLength - 1;
		}
		
		while (ni < pageLength) {
			if (i < target) {
				let opost[ni] = Posts.get(i);
			}
			else {
				let opost[ni] = 0;
			}
			let i = i + 1;
			let ni = ni + 1;
		}
		
		Return opost;
	}
	
	method Array getStats() {
		var Array out;
		let out = Array.new(3);
		let out[0] = totalLength;
		let out[1] = (totalLength /  pageLength) + 1
		let out[2] = cursor + 1;
		Return out;
	}
}
