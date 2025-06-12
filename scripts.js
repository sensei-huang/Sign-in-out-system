<script src=
"https://cdnjs.cloudflare.com/ajax/libs/qrcodejs/1.0.0/qrcode.min.js"></script>
<script>
	/*
	 * -*- coding: utf-8 -*-
	 * version: 1.0.1
	 * updated: 2021/3/5
	 * mail: yunos@graviness.com
	 * home: https://www.graviness.com/
	 * ref.:
	 *  http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/mt.html
	 */
	
	
	/**
	 * Mersenne Twister class
	 * 
	 * Mersenne Twister that is one of a method of generating pseudo-random numbers, is implemented.
	 * 
	 * Generates uniform random numbers of unsigned integer type or floating-point type by the Mersenne Twister.
	 * It uses a single integer to initialize the random number generation,
	 * but can also use arbitrary bit-width values using an array if needed.
	 * 
	 * This class is a JavaScript language port of the following site (C language source).
	 * http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/MT2002/CODES/mt19937ar.c
	 * The external interface is implemented with reference to java.util.Random class of Java(TM) language.
	 * http://sdc.sun.co.jp/java/docs/j2se/1.4/ja/docs/ja/api/java/util/Random.html
	 * 
	 * The performance is about one-half that of the built-in Math.random,
	 * but the quality of the random numbers is as shown in the site concerned.
	 * 
	 * e.g.
	 * // Create an instance and initialize the random number generator with the current time.
	 * var mt = new MersenneTwister(new Date().getTime());
	 * // Uniform random number of 32-bit unsigned integer type.
	 * var randomNumber = mt.nextInteger();
	 */
	function class__MersenneTwister__(){
	var N = 624, M = 397;
	var MAG01 = [0x0, 0x9908b0df];
	
	/**
	 * Generate a new random number generator.
	 * Set the seed according to the argument.
	 * 
	 * @param (None)    Generates a new random number generator.
	 * The current time is used for the seed.
	 * @see Date#getTime()
	 * ---
	 * @param num
	 * @see #setSeed(num)
	 * ---
	 * @param num[]
	 * @see #setSeed(num[])
	 * ---
	 * @param num1, ...
	 * @see #setSeed(num1, ...)
	 */
	MersenneTwister = function()
	{
		this.mt = new Array(N);
		this.mti = N + 1;
	
		var a = arguments;
		switch(a.length) {
		case 0:
			this.setSeed(new Date().getTime());
			break;
		case 1:
			this.setSeed(a[0]);
			break;
		default:
			var seeds = new Array();
			for(var i = 0; i < a.length; ++i)
				seeds.push(a[i]);
			this.setSeed(seeds);
			break;
		}
	};
	
	/**
	 * Set the seed for the random number generator.
	 * 
	 * @param num    Set the seed for the random number generator using a single number.
	 * ---
	 * @param num[]  Set the seed for the random number generator using multiple numbers.
	 * ---
	 * @param num1, ... Set the seed for the random number generator using multiple numbers.
	 */
	MersenneTwister.prototype.setSeed = function()
	{
		var a = arguments;
		switch(a.length) {
		case 1:
			if(a[0].constructor === Number) {
				this.mt[0]= a[0];
				for(var i = 1; i < N; ++i) {
					var s = this.mt[i - 1] ^ (this.mt[i - 1] >>> 30);
					this.mt[i] = ((1812433253 * ((s & 0xffff0000) >>> 16))
							<< 16)
						+ 1812433253 * (s & 0x0000ffff)
						+ i;
				}
				this.mti = N;
				return;
			}
	
			this.setSeed(19650218);
	
			var l = a[0].length;
			var i = 1;
			var j = 0;
	
			for(var k = N > l ? N : l; k != 0; --k) {
				var s = this.mt[i - 1] ^ (this.mt[i - 1] >>> 30)
				this.mt[i] = (this.mt[i]
						^ (((1664525 * ((s & 0xffff0000) >>> 16)) << 16)
							+ 1664525 * (s & 0x0000ffff)))
					+ a[0][j]
					+ j;
				if(++i >= N) {
					this.mt[0] = this.mt[N - 1];
					i = 1;
				}
				if(++j >= l)
					j = 0;
			}
	
			for(var k = N - 1; k != 0; --k) {
				var s = this.mt[i - 1] ^ (this.mt[i - 1] >>> 30);
				this.mt[i] = (this.mt[i]
						^ (((1566083941 * ((s & 0xffff0000) >>> 16)) << 16)
							+ 1566083941 * (s & 0x0000ffff)))
					- i;
				if(++i >= N) {
					this.mt[0] = this.mt[N-1];
					i = 1;
				}
			}
	
			this.mt[0] = 0x80000000;
			return;
		default:
			var seeds = new Array();
			for(var i = 0; i < a.length; ++i)
				seeds.push(a[i]);
			this.setSeed(seeds);
			return;
		}
	};
	
	/**
	 * Generates a pseudo-random numbers.
	 * @param bits  The number of significant bits in the output value.; 0 &lt; bits &lt;= 32.
	 * @return A pseudo-random number.
	 */
	MersenneTwister.prototype.$next = function(bits)
	{
		if(this.mti >= N) {
			var x = 0;
			for(var k = 0; k < N - M; ++k) {
				x = (this.mt[k] & 0x80000000) | (this.mt[k + 1] & 0x7fffffff);
				this.mt[k] = this.mt[k + M] ^ (x >>> 1) ^ MAG01[x & 0x1];
			}
			for(var k = N - M; k < N - 1; ++k) {
				x = (this.mt[k] & 0x80000000) | (this.mt[k + 1] & 0x7fffffff);
				this.mt[k] = this.mt[k + (M - N)] ^ (x >>> 1) ^ MAG01[x & 0x1];
			}
			x = (this.mt[N - 1] & 0x80000000) | (this.mt[0] & 0x7fffffff);
			this.mt[N - 1] = this.mt[M - 1] ^ (x >>> 1) ^ MAG01[x & 0x1];
			this.mti = 0;
		}
		var y = this.mt[this.mti++];
		y ^= y >>> 11;
		y ^= (y << 7) & 0x9d2c5680;
		y ^= (y << 15) & 0xefc60000;
		y ^= y >>> 18;
		return y >>> (32 - bits);
	};
	
	/**
	 * Generate a uniformly distributed boolean pseudo-random number.
	 * @return  true or false
	 */
	MersenneTwister.prototype.nextBoolean = function()
	{
		return this.$next(1) == 1;
	};
	
	/**
	 * Generate a uniformly distributed unsigned 32-bit integer pseudo-random number.
	 * @return  An unsigned 32-bit integer pseudo-random number, between 0 and 4294967295.
	 */
	MersenneTwister.prototype.nextInteger = function()
	{
		return this.$next(32);
	};
	
	/**
	 * Generate a uniformly distributed unsigned 46-bit integer pseudo-random number.
	 * Why 46
	 *  The JavaScript language specification has restrictions regarding number of the bits on operating a bitwise and integer.
	 *  In this implementation of the Mersenne Twister, where high speed was important,
	 *  the operation result was broken at 48 bits or more.
	 *  Therefore, we decided to use the safe 46 bits.
	 * @return  An unsigned 46-bit integer pseudo-random number, between 0 and 70368744177663.
	 */
	MersenneTwister.prototype.nextLong = function()
	{
		// NOTE: The calculation result will be broken if it is more than 48 bits.
		// (46 - 32) = 14 = [7] + [7], 32 - [7] = [25], 32 - [7] = [25]
		// 2^(46 - [25]) = 2^21 = [2097152]
		return this.$next(25) * 2097152 + this.$next(25);
	};
	
	/**
	 * Generate a 32-bit floating-point pseudo-random number with uniform distribution in the range 0.0 to 1.0.
	 * @return  [0.0 1.0) of the half-open section.
	 */
	MersenneTwister.prototype.nextFloat = function()
	{
		return this.$next(32) / 4294967296.0; // 2^32
	};
	
	/**
	 * Generate a 46-bit floating-point pseudo-random number with uniform distribution in the range 0.0 to 1.0.
	 * Why 46 --> See #nextLong
	 * @return  [0.0 1.0) of the half-open section.
	 */
	MersenneTwister.prototype.nextDouble = function()
	{
		return (this.$next(25) * 2097152 + this.$next(25))
			/ 70368744177664.0; // 2^46
	};
	
	} class__MersenneTwister__();
	
	
	
	/**
	* Generate a floating-point pseudo-random numbers
	* in the half-open section [0 1.0) using Mersenne Twister.
	* It is used in the same way as Math.random.
	* 
	* e.g.
	* // Generate a floating-point value that is greater than or equal to 0 and less than 1.
	* var r = Math.mersenneTwisterRandom();
	*/
	Math.mersenneTwisterRandom = function()
	{
	Math.__MERSENNE_TWISTER__ = new MersenneTwister();
	
	return function() {
		return Math.__MERSENNE_TWISTER__.nextFloat();
	}
	}();
</script>
<script>
var isDown = false;
var initWidth = parseInt(document.getElementsByClassName("left")[0].clientWidth)+30;
var address;

function getCard(){
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/address", true);
  xhr.onreadystatechange = function() {
    if(xhr.readyState == 4 && xhr.status == 200){
      address = xhr.responseText;
    }
  };
  xhr.send(null);
}

function mdown(e){
  isDown = true;
}

function mup(e){
  isDown = false;
}

function mmove(e){
  var ex, ey;
  if(e.type == 'touchmove'){
    var touch = event.touches[0];
    ex = touch.pageX;
    ey = touch.pageY;
  } else if (e.type == 'mousemove') {
    ex = e.clientX;
    ey = e.clientY;
  }
  e.preventDefault();
  if(isDown){
    var x = Math.min(Math.max(ex, initWidth), parseInt(document.body.offsetWidth)-350);
    document.getElementById("dragbar").style.left = x+"px";
    document.getElementsByClassName("left")[0].style.width = x+"px";
    var size = parseInt(document.body.offsetWidth)-x-50;
    document.getElementsByClassName("right")[0].style.width = parseInt(document.body.offsetWidth)-x+"px";
    document.getElementById("qrcode").innerHTML = "";
    new QRCode(document.getElementById("qrcode"), {
      text: window.location.href+"?code="+generateCode(),
      width: size,
      height: size
    });
  }
}

function initDragBar(){
  document.getElementById("dragbar").style.left = initWidth+"px";
  document.getElementsByClassName("left")[0].style.width = initWidth+"px";
  document.getElementsByClassName("right")[0].style.width = parseInt(document.body.offsetWidth)-initWidth-30+"px";
  
  document.getElementById("dragbar").addEventListener('mousedown', (e) => {mdown(e)}, true);
  document.getElementById("dragbar").addEventListener('touchstart', (e) => {mdown(e)}, true);
  
  document.addEventListener('mouseup', (e) => {mup(e)}, true);
  document.addEventListener('touchend', (e) => {mup(e)}, true);
  
  document.addEventListener('mousemove', (e) => {mmove(e)}, true);
  document.addEventListener('touchmove', (e) => {mmove(e)}, true);
  
  document.getElementById("dragbar").click();
}

function removeElemDelay(element, delay) {
  setTimeout(() => {
    element.style.animation = "fade-out 0.5s ease-in";
    element.style.opacity = 1;
    setTimeout(() => {
      if(element && element.parentNode){
        element.style.display = "none";
        element.remove();
      }
    }, 500);
  }, delay);
}

function signChange(){
  var inoutstr = document.getElementById("sign").value;
  var outinstr;
  if(inoutstr == "in"){
  	outinstr = "out";
  }else{
    outinstr = "in";
  }
  document.getElementById(inoutstr+"reason").hidden = false;
  document.getElementById(inoutstr+"reason").required = true;

  document.getElementById(outinstr+"reason").hidden = true;
  document.getElementById(outinstr+"reason").required = false;
  
  document.getElementById("other").hidden = true;
  document.getElementById("other").required = false;
  
  document.getElementById("email").hidden = false;
  document.getElementById("outdef").selected = true;
  document.getElementById("indef").selected = true;
  document.getElementById("submit").hidden = false;
}

function reasonChange(inout){
  var inoutstr;
  if(inout == 1){
    inoutstr = "in";
  }else{
    inoutstr = "out";
  }
  if(document.getElementById(inoutstr+"reason").value == "other"){
    document.getElementById("other").hidden = false;
    document.getElementById("other").required = true;
    document.getElementById("other").value = "";
  }else{
    document.getElementById("other").hidden = true;
    document.getElementById("other").required = false;
  }
}

async function submitcode(){
  var inout = document.getElementById("sign").value;
  var email = document.getElementById("email").value;
  var reason = document.getElementById(inout+"reason").value;
  if(reason == "other"){
  	reason = document.getElementById("other").value;
  }
  var aler = document.createElement("div");
  aler.className = "sendingalert";
  aler.innerHTML = '<span class="closebtn" onclick=\'this.style.display = "none"; removeElemDelay(this.parentElement, 0); this.parentElement.style.animation = "fade-out 0.5s ease-in"; \'>&times;</span><strong>Sending...</strong>';
  document.getElementById("contain").appendChild(aler);
  if(await sheetLog(inout, email, reason)){
    removeElemDelay(aler, 0);
    aler = document.createElement("div");
    aler.className = "alert";
    aler.innerHTML = '<span class="closebtn" onclick=\'this.style.display = "none"; removeElemDelay(this.parentElement, 0); this.parentElement.style.animation = "fade-out 0.5s ease-in"; \'>&times;</span><strong>Successfully signed '+inout+'!</strong> Email: '+email+', Reason: '+reason+'.';
    document.getElementById("contain").appendChild(aler);
    removeElemDelay(aler, 4000);
  }else{
    removeElemDelay(aler, 0);
    aler = document.createElement("div");
    aler.className = "failalert";
    aler.innerHTML = '<span class="closebtn" onclick=\'this.style.display = "none"; removeElemDelay(this.parentElement, 0); this.parentElement.style.animation = "fade-out 0.5s ease-in"; \'>&times;</span><strong>Failed to sign in! Please try again.';
    document.getElementById("contain").appendChild(aler);
    removeElemDelay(aler, 4000);
  }
}

function convertBase(value, from_base, to_base) {
  var range = '0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ'.split('');
  var from_range = range.slice(0, from_base);
  var to_range = range.slice(0, to_base);
  
  var dec_value = value.split('').reverse().reduce(function (carry, digit, index) {
    if (from_range.indexOf(digit) === -1) throw new Error('Invalid digit `'+digit+'` for base '+from_base+'.');
    return carry += from_range.indexOf(digit) * (Math.pow(from_base, index));
  }, 0);
  
  var new_value = '';
  while (dec_value > 0) {
    new_value = to_range[dec_value % to_base] + new_value;
    dec_value = (dec_value - (dec_value % to_base)) / to_base;
  }
  return new_value || '0';
}

function generateCode(){
  var d = new Date();
  let seed = d.getDate()+d.getMonth()*31+d.getFullYear()*372;
  var MT = new MersenneTwister(seed);
  return convertBase(String(MT.nextLong()), 10, 36).slice(-4);
}

var SCRIPT_URL = "https://script.google.com/macros/s/AKfycbx6l7S2KUZZG_v6Ks1nLkFMGOZRrxmIVK6B2g3rSN0bHsBp82ss2MmiUAgrwqYBC08R/exec";

async function sheetLog(inout, name, reason){
  try {
    const response = await fetch(SCRIPT_URL+"?met=1&inout="+inout+"&name="+name+"&reason="+reason, {mode: 'no-cors'});
    const tex = await response.text();
    if(tex == "ERROR"){
      console.error("Error occured form google sheets api when calling function sheetLog");
      return false;
    }
    return true;
  } catch (error) {
    console.error(error.message);
    return false;
  }
  return true;
}

async function cardLog(name, data){
  try {
    const response = await fetch(SCRIPT_URL+"?met=2&name="+name+"&data="+data, {mode: 'no-cors'});
    const tex = await response.text();
    if(tex == "ERROR"){
      console.error("Error occured form google sheets api when calling function sheetLog");
      return false;
    }else{
      return true;
    }
  } catch (error) {
    console.error(error.message);
  }
  return false;
}

async function getData() {
  try {
    const response = await fetch(SCRIPT_URL+"?met=0");
    const tex = await response.text();
    if(tex == "ERROR"){
      console.error("Error occured form google sheets api when calling function getData");
      return;
    }
    document.getElementById("content").innerHTML = tex;
  } catch (error) {
    console.error(error.message);
  }
}

var height = parseInt(document.body.offsetWidth)-initWidth-50;
new QRCode(document.getElementById("qrcode"), {
  text: window.location.origin+"/?code=",
  width: height,
  height: height
});
document.getElementById("verifycode").innerHTML = "Verification code: "+generateCode();
initDragBar();
setInterval(getCard, 300);
</script>
