function status_viewer_t(div)
{
	if(!div)
		return null;

	this.div=div;
	this.el=document.createElement("div");
	this.div.appendChild(this.el);

	var _this=this;
	this.connection=new connection_t(this.el,function(message){_this.show(message);});

	this.el.appendChild(document.createElement("br"));
	this.el.style.width="90%"
	this.el.style.height="80%"
	this.el.style.margin="10px";
	this.el.style.marginLeft="auto";
	this.el.style.marginRight="auto";
	this.el.style.padding="0px";

	this.textarea=document.createElement("textarea");
	this.el.appendChild(this.textarea);
	this.textarea.wrap="off";
	this.textarea.readOnly=true;
	this.textarea.style.resize="none";
	this.textarea.style.width="100%";
	this.textarea.style.height="100%";
}

status_viewer_t.prototype.destroy=function()
{
	this.connection.destroy();
	this.div.removeChild(this.el);
}

status_viewer_t.prototype.show=function(message)
{
	this.textarea.innerHTML+=message+"\n";
	this.textarea.scrollTop=this.textarea.scrollHeight;
}
