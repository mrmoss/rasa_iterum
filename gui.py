#!/usr/bin/env python
import wx

class window_t:
	def __init__(self):
		self.prompt_text="You are connected to a robot, disconnect?"

		self.app=wx.App(redirect=True)
		self.create_frame_m("Rasa Iterum")

		self.create_status_m()
		self.set_status("Test")

		self.create_menu_bar_m()
		self.create_file_menu_m()
		self.create_help_menu_m()

		self.create_panel_box_m()

		self.b_text=wx.StaticText(self.panel,-1,"Hello World!")
		self.box.Add(self.b_text,0,wx.ALL,10)
		self.b_text.SetFont(wx.Font(14,wx.SWISS,wx.NORMAL,wx.BOLD))
		self.b_text.SetSize(self.b_text.GetBestSize())

		self.b_close=wx.Button(self.panel,wx.ID_CLOSE,"Close")
		self.box.Add(self.b_close,0,wx.ALL,10)
		self.b_close.Bind(wx.EVT_BUTTON,self.on_quit_m)

		self.show_m()

	def set_status(self,text):
		self.statusbar.SetStatusText(text)

	def on_quit_m(self,event):
		if len(self.prompt_text)>0:
			dialog=wx.MessageDialog(self.frame,self.prompt_text,
				"Confirm",wx.OK|wx.CANCEL|wx.ICON_QUESTION)
			result=dialog.ShowModal()
			dialog.Destroy()

			if result==wx.ID_OK:
				self.frame.Destroy()
		else:
			self.frame.Destroy()

	def on_about_m(self,event):
		dialog=wx.MessageDialog(self.frame,"Rasa Iterum about window...",
			"About",wx.OK|wx.ICON_QUESTION)
		result=dialog.ShowModal()
		dialog.Destroy()

	def ignore_m(self,event):
		None

	def create_frame_m(self,title):
		self.frame=wx.Frame(None,title=title)
		self.frame.Bind(wx.EVT_CLOSE,self.on_quit_m)
		self.frame.Bind(wx.EVT_MENU_HIGHLIGHT,self.ignore_m)

	def create_status_m(self):
		self.statusbar=self.frame.CreateStatusBar()

	def create_menu_bar_m(self):
		self.menu_bar=wx.MenuBar()
		self.frame.SetMenuBar(self.menu_bar)

	def create_file_menu_m(self):
		self.file_menu=wx.Menu()
		self.menu_bar.Append(self.file_menu,"&File")

		self.file_menu_quit=self.file_menu.Append(wx.ID_EXIT,text="&Quit",)
		self.frame.Bind(wx.EVT_MENU,self.on_quit_m,self.file_menu_quit)

	def create_help_menu_m(self):
		self.help_menu=wx.Menu()
		self.menu_bar.Append(self.help_menu,"&Help")

		self.m_help_menu_about=self.help_menu.Append(wx.ID_ABOUT,text="&About",)
		self.frame.Bind(wx.EVT_MENU,self.on_about_m,self.m_help_menu_about)

	def create_panel_box_m(self):
		self.box=wx.BoxSizer(wx.VERTICAL)
		self.panel=wx.Panel(self.frame)
		self.panel.SetSizer(self.box)
		self.panel.Layout()

	def show_m(self):
		self.frame.Show()
		self.app.MainLoop()

if __name__=="__main__":
	window=window_t()