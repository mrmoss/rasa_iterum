function gui_t(div)
{
	if(!div)
		return null;

	this.div=div;
	this.el=document.createElement("div");
	this.div.appendChild(this.el);

	var _this=this;

	this.connection=new connection_t(this.el,
		function(message){_this.status_viewer.show(message);});

	this.serial_selector=new serial_selector_t(this.el,
		function(port){_this.connection.connect(port);},
		function(port){_this.connection.disconnect(port);});
	this.connection.on_disconnect=function(){_this.serial_selector.disconnect();};

	this.status_viewer=new status_viewer_t(this.el);

	//TEST CODE
	/*setTimeout(function(){_this.connection.reconfigure({c:{i:[15]},o:[5,6]});},10000);

	function connection_t.prototype.download_config=function()
	{
		ss_get("127.0.0.1:8081","test",
		function(json)
		{
		},
		function(error)
		{
			console.log(error);
		});
	}*/
}

gui_t.prototype.destroy=function()
{
	this.connection.destroy();
	this.status_viewer.destroy();
}