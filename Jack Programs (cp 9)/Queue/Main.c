class Main {
	function void Main() {
		var PostDB Posts;
		var boolean shows,showh;
		let Posts = PostDB.new(0,2,20,10);
		let shows = true;
		let showh = true;
		
		while (true) {
			if (showh) {
				do drawUI();
			}
			if (shows) {
				do Output.moveCursor(22, 0);
				do Posts.printStats();
			}
			
			do Posts.drawPosts();
			
			var char a;
			var int err;
			let err = 0;
			let a = readChar();
			if (a = 49) {
				// create new and set
				var String ptitle, pbody;
				do Output.moveCursor(22, 0);
				let ptitle = Keyboard.readLine("Enter post title: ");
				do Output.moveCursor(22, 0);
				let pbody = Keyboard.readLine("Enter post body: ");
				
				do Posts.add(ptitle,pbody);
			}
			else {
				if (a = 50) {
					// Remove top post
					let err = Posts.rem();
				}
				else {
					if (a = 51){
						// set post
					}
					else {
						if (a = 52) {
							let s = readInt("Delete post #(1-10): ") - 1
							do a[s].dispose();
							// Delete post using index
						}
						else {
							// notify error
						}
					}
				}
			}
			
			do Output.moveCursor(22, 0);
			if (err = 0) {
				do Output.printString("Success");
			}
			else {
				if (err = 1) {
					do Output.printString("Exception");
				}
				else {
					do Output.printString("Error");
				}
			}
		}
		return;
	}
	
	function drawUI() {
		do Screen.setColor(false);
		do Output.moveCursor(0,28);
		do Output.printString("Queue App");
		do Output.printLn();
		do Output.printString("0:Tog 1:Add 2:Rem 3:Set 4:Del 5:Ins 6:Mov 7:Swi 8:Up 9:Dwn")
		do Output.printLn();
		do PostDB.underline(1);
		return;
	}
}