//state {disconnected=0,opening,firmware_check,configure,connected}
//on_message - callback triggered when an error or status change is added.
function connection_t(div,on_message)
{
	if(!div)
		return null;

	this.div=div;
	this.el=document.createElement("div");
	this.div.appendChild(this.el);
	this.on_message=on_message;

	var _this=this;

	this.serial=null;
	this.serial_name=null;
	this.serial_proc=null;
	this.parser=new parser_t(function(str){_this.on_packet_m(str);});
	this.serial_selector=new serial_selector_t(this.el,
		function(port){_this.on_selector_connect_m(port);},
		function(port){_this.on_selector_disconnect_m(port);});
	this.state=0;
	this.timeout=null;

	chrome.serial.onReceive.addListener(function(info){_this.on_receive_m(info);});
	chrome.serial.onReceiveError.addListener(function(info){_this.on_receive_error_m(info);});
}

connection_t.prototype.destroy=function()
{
	this.disconnect();
	this.serial_selector.destroy();
	this.div.removeChild(this.el);
}

connection_t.prototype.disconnect=function()
{
	this.parser.reset();
	this.serial_selector.disconnect();
	this.serial_name=null;
	this.serial_proc=null;

	if(this.serial)
	{
		chrome.serial.disconnect(this.serial.connectionId,
			function()
			{
				if(chrome.runtime.lastError)
				{}
			});

		this.serial=null;
	}

	this.reset_timeout_m();

	this.state=0;
}


connection_t.prototype.check_firmware_m=function()
{
	if(this.serial&&this.state==2)
	{
		this.send_m("{s:{}}",true);

		if(this.on_message)
			this.on_message("Checking firmware on \""+this.serial_name+"\".");

		this.reset_timeout_m();
		var _this=this;
		this.timeout=setTimeout(function()
		{
			if(_this.state==2)
				_this.raise_error_m();
		},2000);
	}
}

connection_t.prototype.configure_m=function(config)
{
	if(this.serial)
		this.send_m(config,true);
}

connection_t.prototype.on_connect_m=function(connection_info)
{
	if(chrome.runtime.lastError)
	{
		this.raise_error_m(chrome.runtime.lastError.message);
		return;
	}

	this.serial=connection_info;
	var _this=this;

	if(this.on_message)
		this.on_message("Flushing \""+this.serial_name+"\".");

	chrome.serial.flush(connection_info.connectionId,
		function(connection_info){_this.on_flushed_m(connection_info);});
}

connection_t.prototype.on_flushed_m=function(connection_info)
{
	if(chrome.runtime.lastError||!this.serial)
	{
		this.raise_error_m(chrome.runtime.lastError.message);
		return;
	}

	if(!this.serial)
	{
		this.raise_error_m();
		return;
	}

	if(this.state==1)
	{
		this.state=2;
		var _this=this;
		this.reset_timeout_m();
		this.timeout=setTimeout(function(){_this.check_firmware_m();},5000);
	}
}

connection_t.prototype.on_packet_m=function(str)
{
	if(this.serial)
	{
		try
		{
			var json=JSON.parse(str);

			if(this.state==2&&json.p&&json.m)
			{
				this.state=3;
				this.reset_timeout_m();
				this.serial_proc=json.p;

				if(this.on_message)
					this.on_message("Found "+json.p+" with "+json.m+" free bytes on \""+
						this.serial_name+"\".");

				if(this.on_message)
					this.on_message("Configuring \""+this.serial_name+"\".");

				this.configure_m("{c:{i:[14,15]},o:[5,6]}");
			}
			else if(this.state==3&&json.c)
			{
				this.state=4;
				this.reset_timeout_m();

				if(this.on_message)
					this.on_message("Connected to \""+this.serial_name+
						"\" ("+this.serial_proc+").");

				this.send_m("{u:{o:[1,0]}}");
			}
			else if(this.state==4)
			{
				if(this.on_message)
					this.on_message(str);

				this.send_m("{u:{o:[1,0]}}");
			}
			else if(this.state>1)
			{
				this.raise_error_m();
			}
		}
		catch(error)
		{
			if(this.state!=4)
				this.disconnect();
		}
	}
}

connection_t.prototype.on_selector_connect_m=function(port)
{
	if(this.state==0)
	{
		this.state=1;
		var _this=this;
		this.serial_name=port;

		if(this.on_message)
			this.on_message("Attempting to connect to \""+port+"\".");

		chrome.serial.connect(port,{bitrate:57600},
			function(connection_info){_this.on_connect_m(connection_info);});
	}
}

connection_t.prototype.on_selector_disconnect_m=function(port)
{
	if(this.serial)
	{
		this.disconnect();

		if(this.on_message)
			this.on_message("Disconnected.");
	}
}

connection_t.prototype.on_receive_m=function(info)
{
	this.parser.parse(info.data);
}

connection_t.prototype.on_receive_error_m=function(error)
{
	this.disconnect();
}

connection_t.prototype.raise_error_m=function(message)
{
	var str="";

	if(this.state==1)
		str+="Could not open serial port";
	else if(this.state==2)
		str+="Firmware is not responding";
	else if(this.state==3)
		str+="Could not configure";
	else
		str+="Unknown error";

	if(message)
		str+=" (\""+message+"\").";
	else
		str+=".";

	if(this.on_message)
		this.on_message(str);

	this.disconnect();
}

connection_t.prototype.reset_timeout_m=function()
{
	if(this.connection_timeout)
	{
		clearTimeout(this.connection_timeout);
		this.connection_timeout=null;
	}
}

connection_t.prototype.send_m=function(str,disconnect)
{
	var _this=this;

	chrome.serial.send(this.serial.connectionId,build_packet(str),function(send_info)
	{
		if(chrome.runtime.lastError&&disconnect)
			_this.raise_error_m(chrome.runtime.lastError.message);
	});
}