class Main {
	function void Main() {
		var Array Posts;
		
		var int proy,prox;
		let proy = 23;
		let prox = 0;
		
		let Posts = Array.new(11);
		var int i, s;
		let i = 0;
		var char a;
		while (true) {
			do drawUI();
			let a = readChar();
			if (a = 49) {
				// create new and set
				var String ptitle, pbody;
				do Output.moveCursor(proy, prox);
				let title = Keyboard.readLine("Enter post title: ");
				do Output.moveCursor(proy, prox);
				let body = Keyboard.readLine("Enter post body: ");
				
				let Posts[i] = Post.new();
				do Posts[i].display();
				do underLine((i * 2) + 4);
				let i = i + 1;
			}
			else {
				if (a = 50) {
					// Remove top post
					let i = i - 1;
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
		}
		return;
	}
	
	function drawUI() {
		do Screen.setColor(false);
		do Output.moveCursor(0,28);
		do Output.printString("Queue App");
		do Output.printString("1:Add 2:Remove 3:Set 4:Delete 5:Move 6:Switch 7:Up 8:Down")
		do Output.printLn();
		do PostDB.underline(1);
		return;
	}
}
