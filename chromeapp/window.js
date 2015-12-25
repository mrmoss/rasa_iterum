var div=null;
var connection=null;
var status_viewer=null;

function on_load()
{
	div=document.getElementById("content");
	status_viewer=new status_viewer_t(div);
}

on_load();