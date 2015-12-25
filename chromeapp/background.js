function on_launch()
{
	chrome.app.window.create('window.html',{'outerBounds':{'width':400,'height':320}});
}

chrome.app.runtime.onLaunched.addListener(on_launch);
