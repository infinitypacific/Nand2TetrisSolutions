//Queue app
//Programmed by Lucas Lux aka Pacifiky
//23 total lines
//58 total chars
//ToDo
//!Menu
//!Toggle ui
//!Replace yn with numselect
//!Add exit to numselect p/p
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
			
			do Posts.drawPosts();
			
			if (shows) {
				do Output.moveCursor(22, 0);
				do Posts.printStats();
			}
			
			var int err,a;
			let err = 0;
			let a = Keyboard.readInt("");
			if (a = 0) {
				//Toggle logic
				var int sel;
				do Output.moveCursor(22, 0);
				let sel = Keyboard.readInt("Toggle: 0:Exit 1:Header 2:Stats 3:Prefix");
				if(sel = 1){
					let showh = !showh;
					if(showh){
						Posts.setOffset(0,2);
						Posts.setPage(10);
					}
					else{
						Posts.setOffset(0,0);
						Posts.setPage(11);
					}
				}
				else {
					if(sel = 2){
						let shows = !shows;
					}
					else {
						if(sel = 3){
							var int presel;
							do Output.moveCursor(22, 0);
							let presel = Keyboard.readInt("Prefix: 0:Exit 1:Null 2:Numeral 3:Bullet 4:Custom");
							if(presel = 1 | presel = 2 | presel = 3){
								err = Posts.setPrefix(presel-1);
							}
							else{
								if(presel = 4){
									var char cb;
									do Output.moveCursor(22, 0);
									do Output.printString("Custom bullet (Enter char): ");
									let cb = Keyboard.readChar();
									do Posts.setBullet(cb);
								}
								else{
									if(!(presel = 0)){
										err = 1;
									}
								}
							}
						} else {
							if(!(sel = 0)){
								err = 1;
							}
						}
					}
				}
			}
			else {
				if (a = 1) {
					// create new and set
					var String ptitle, pbody;
					do Output.moveCursor(22, 0);
					let ptitle = Keyboard.readLine("Enter post title: ");
					do Output.moveCursor(22, 0);
					let pbody = Keyboard.readLine("Enter post body: ");
					
					do Posts.add(ptitle,pbody);
				}
				else {
					if (a = 2){
						// Remove top post
						let err = Posts.rem();
					}
					else {
						if (a = 3) {
							//Setpost
							var String ptitle, pbody;
							var int index;
							do Output.moveCursor(22, 0);
							let index = Keyboard.readInt("Enter set index: ");
							do Output.moveCursor(22, 0);
							let ptitle = Keyboard.readLine("Enter post title: ");
							do Output.moveCursor(22, 0);
							let pbody = Keyboard.readLine("Enter post body: ");
							let err = Posts.set(index,ptitle,pbody);
						}
						else {
							if (a = 4) {
								//Del
								var int index;
								do Output.moveCursor(22, 0);
								let index = Keyboard.readInt("Enter del index: ");
								let err = Posts.del(index);
							}
							else {
								if (a = 5) {
									//Ins
									var String ptitle, pbody;
									var int index;
									do Output.moveCursor(22, 0);
									let index = Keyboard.readInt("Enter ins index: ");
									do Output.moveCursor(22, 0);
									let ptitle = Keyboard.readLine("Enter post title: ");
									do Output.moveCursor(22, 0);
									let pbody = Keyboard.readLine("Enter post body: ");
									let err = Posts.ins(index,ptitle,pbody);
								}
								else {
									if (a = 6) {
										//Mov
										var int ind1,ind2;
										do Output.moveCursor(22, 0);
										let ind1 = Keyboard.readInt("Enter mov from: ");
										do Output.moveCursor(22, 0);
										let ind2 = Keyboard.readInt("Enter mov to: ");
										let err = Posts.mov(ind1,ind2);
									}
									else {
										if (a = 7) {
											//Swi
											var int ind1,ind2;
											do Output.moveCursor(22, 0);
											let ind1 = Keyboard.readInt("Enter swi1: ");
											do Output.moveCursor(22, 0);
											let ind2 = Keyboard.readInt("Enter swi2: ");
											let err = Posts.swi(ind1,ind2);
										}
										else {
											if (a = 8) {
												//Up
												var char sel;
												do Output.moveCursor(22, 0);
												do Output.printString("Up 0:Exit 1:Post 2:Page");
												let sel = Keyboard.readChar();
												if(sel = "2){
													let err = Posts.up(true);
												}
												else {
													if(sel = "1){
														let err = Posts.up(false);
													}
													else {
														if(!(sel = "0)){
															let err = 1;
														}
													}
												}
											}
											else {
												if (a = 9) {
													//Dwn
													var char sel;
													do Output.moveCursor(22, 0);
													do Output.printString("Dwn 0:Exit 1:Post 2:Page");
													let sel = Keyboard.readChar();
													if(sel = "2){
														let err = Posts.dwn(true);
													}
													else {
														if(sel = "1){
															let err = Posts.dwn(false);
														}
														else {
															if(!(sel = "0)){
																let err = 1;
															}
														}
													}
												}
												else {
													//do Output.printString("Error: INV SEL");
													let err = 2;
												}
											}
										}
									}
								}
							}
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
	
	function void drawUI() {
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