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

	this.download_config();
	setInterval(function(){_this.download_config();},2000);
}

gui_t.prototype.destroy=function()
{
	this.connection.destroy();
	this.status_viewer.destroy();
}

gui_t.prototype.download_config=function()
{
	var superstar="127.0.0.1:8081";
	var path="test";

	this.status_viewer.show("Downloading config \""+path+"\" from "+superstar+".");

	var _this=this;

	ss_get(superstar,path,
		function(json)
		{
			var was_empty=false;

			if(!json.c)
			{
				was_empty=true;
				json.c={};
				ss_set(superstar,path,json,function(){},function(error)
				{
					_this.status_viewer.show(error);
				});
			}

			if(JSON.stringify(json)!=_this.connection.config)
			{
				if(was_empty)
					_this.status_viewer.show("Empty config, generating default.");
				else
					_this.status_viewer.show("Got configuration \""+JSON.stringify(json)+"\".");

				if(_this.connection.state==4)
						_this.connection.reconfigure(json);
				else
					_this.connection.set_config(json);
			}
		},
		function(error)
		{
			_this.status_viewer.show(error);
		});
}