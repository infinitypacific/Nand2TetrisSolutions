﻿/** The list is like an array but you can append to it, append array or lists to the end. This works by deleting original Array and making a new one.*/
class List {
	field Array l;
	field int length;
	
	constructor List new(int len) {
		let l = Array.new(len);
		let length = len;
		Return this;
	}
	
	method void dispose() {
		do l.dispose();
		do Memory.deAlloc(this);
		return;
	}
	
	// if int doesn work for obj and primitives, use an array to do a makeshift "auto" format
	method void set(int i, int v) {
		let l[i] = v;
		return;
	}
	
	method int get(int i) {
		Return l[i];
	}
	
	method int getLen() {
		Return length;
	}
	
	method void append(int v) {
		var Array n;
		var int i;
		let i = 0;
		let n = Array.new(length + 1);
		let n[length] = v;
		while (i < length) {
			let n[i] = l[i];
			let i = i + 1;
		}
		let length = length + 1;
		do l.dispose();
		let l = n;
		return;
	}
	
	method void appendList(List v) {
		var int alen, io, iv;
		var Array n;
		let alen = v.getLen() + length;
		let n = Array.new(alen);
		let io = 0;
		let iv = length;
		while (io < length) {
			let n[io] = l[io]
			let io = io + 1;
		}
		while (iv < alen) {
			let n[iv] = v[iv-length];
			let iv = iv + 1;
		}
		
		let length = alen;
		do l.dispose();
		let l = n;
		return;
	}
	
	method void setLen(int nlen) {
		if (nlen = length) {
			return;		
		}
		var Array n;
		var int i;
		let n = Array.new(nlen);
		let i = 0;
		
		while ((i < nlen) and (i < length)) {
			let n[i] = l[i];
			let i = i + 1;
		}
		
		do l.dispose();
		let l = n;
		return;
	}
}
