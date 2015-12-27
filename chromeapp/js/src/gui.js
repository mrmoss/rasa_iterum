function gui_t(div)
{
	if(!div)
		return null;

	this.div=div;
	this.el=document.createElement("div");
	this.div.appendChild(this.el);
	this.superstar_errored=false;

	var _this=this;

	this.connection=new connection_t(this.el,
		function(message){_this.status_viewer.show(message);});

	this.serial_selector=new serial_selector_t(this.el,
		function(port){_this.connection.connect(port);},
		function(port){_this.connection.disconnect(port);});
	this.connection.on_disconnect=function(){_this.serial_selector.disconnect();};

	this.status_viewer=new status_viewer_t(this.el);

	this.download_config();
	setInterval(function(){_this.download_config();},2000);
	this.download_write();
}

gui_t.prototype.destroy=function()
{
	this.connection.destroy();
	this.status_viewer.destroy();
}

gui_t.prototype.download_config=function()
{
	var superstar="127.0.0.1:8081";
	var path="test2/config";
	var _this=this;

	ss_get(superstar,path,
		function(json)
		{
			if(_this.superstar_errored||JSON.stringify(json)!=JSON.stringify(_this.connection.config))
			{
				_this.status_viewer.show("Got configuration \""+JSON.stringify(json)+"\".");
				_this.connection.reconfigure(json);
			}

			_this.superstar_errored=false;
		},
		function(error)
		{
			_this.status_viewer.show(error);
			_this.connection.disconnect();
			_this.superstar_errored=true;
		});
}

gui_t.prototype.download_write=function()
{
	var _this=this;

	if(this.connection.state!=0)
	{
		var superstar="127.0.0.1:8081";
		var path="test2/write";

		ss_get(superstar,path,
			function(json)
			{
				if(_this.connection.set_write(json))
					_this.status_viewer.show("Got write \""+JSON.stringify(json)+"\".");

				setTimeout(function(){_this.download_write();},10);
			},
			function(error)
			{
				_this.status_viewer.show(error);
				_this.connection.disconnect();
				setTimeout(function(){_this.download_write();},10);
				_this.superstar_errored=true;
			});
	}
	else
	{
		setTimeout(function(){_this.download_write();},10);
	}
}