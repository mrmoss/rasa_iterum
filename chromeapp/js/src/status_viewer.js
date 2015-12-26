function status_viewer_t(div)
{
	if(!div)
		return null;

	this.div=div;
	this.el=document.createElement("div");
	this.div.appendChild(this.el);

	this.textarea=document.createElement("textarea");
	this.el.appendChild(this.textarea);
	this.textarea.wrap="off";
	this.textarea.readOnly=true;
	this.textarea.style.resize="none";
	this.textarea.style.width="370px";
	this.textarea.style.height="350px";

	this.last_message="";
}

status_viewer_t.prototype.destroy=function()
{
	this.div.removeChild(this.el);
}

status_viewer_t.prototype.show=function(message)
{
	if(message!=this.last_message)
	{
		this.last_message=message;
		this.textarea.innerHTML+=message+"\n";
		this.textarea.scrollTop=this.textarea.scrollHeight;
	}
}
