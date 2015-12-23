import arduino
from arduino import millis
import firmware
import packetize
import re
import serial
import threading
import time
import wx

class window_t:
	def __init__(self):
		self.arduino=arduino.arduino_t()
		self.parser=packetize.parser_t()
		self.state_timer_m=0
		self.old_arduino_name=""
		self.old_port_list=[]
		self.prompt_text=""
		self.mutex=threading.Lock()

		self.app=wx.App(redirect=True)
		self.create_frame_m("Rasa Iterum")

		self.create_status_m()
		self.set_status('Disconnected.')

		self.create_menu_bar_m()
		self.create_file_menu_m()
		self.create_help_menu_m()

		self.create_panel_box_m()

		self.create_timer_m()

		self.create_gui_m()

		self.show_m()

	def set_status(self,text):
		self.statusbar.SetStatusText(text)

	def connect(self):
		self.b_connect.SetLabel("Disconnect")
		self.b_combo.Enable(False)
		self.old_arduino_name=self.b_combo.GetValue()

		try:
			self.set_status('Attempting to connect to "'+self.old_arduino_name+'"...')
			self.mutex.acquire()
			self.arduino.connect(self.old_arduino_name)
			self.mutex.release()

			time.sleep(1)
			self.set_status('Checking firmware...')
			time.sleep(1)

			self.mutex.acquire()
			status=firmware.get_status_blocking(self.arduino,self.parser)
			self.mutex.release()
			self.set_status('Firmware confirmed ('+status['p']+' with '+str(status['m'])+' bytes of free RAM).')

			time.sleep(1)
			self.set_status('Sending configuration...')
			time.sleep(1)

			self.mutex.acquire()
			firmware.send_configuration(self.arduino,self.parser,'{c:{}}')
			self.mutex.release()
			self.set_status('Configuration verified.')

			time.sleep(1)
			self.set_status('Connected ('+status['p']+' on \"'+self.old_arduino_name+'\").')

		except Exception as error:
			self.mutex.release()
			self.set_status(str(error))
			self.disconnect(False)

	def disconnect(self,update_status=True):
		self.mutex.acquire()
		self.arduino.close()
		self.mutex.release()

		self.parser.reset()
		self.old_arduino_name=""
		self.prompt_text=""

		self.b_connect.SetLabel("Connect")

		if update_status:
			self.set_status('Disconnected.')

	def on_connect_m(self,event):
		if self.arduino.is_opened():
			self.disconnect()
		else:
			self.modify_input_m(self.i_school)
			self.modify_input_m(self.i_robot)
			self.thread=threading.Thread(target=self.connect)
			self.thread.start()

	def on_timer_m(self,event):
		port_list=self.arduino.list()
		port_list.sort()

		enabled=True
		disconnect_enabled=False
		old_selected=self.b_combo.GetValue()

		if self.arduino.is_opened() and self.old_arduino_name in port_list:
			enabled=False

		if set(self.old_port_list)!=set(port_list):
			self.old_port_list=port_list
			self.b_combo.Clear()
			in_list=False

			for ii in range(0,len(port_list)):
				self.b_combo.Append(port_list[ii])

				if old_selected==port_list[ii]:
					self.b_combo.SetSelection(ii)
					in_list=True

			if len(port_list)>0 and not in_list:
				self.b_combo.SetSelection(0)

		if len(port_list)<=0 or not self.old_arduino_name in port_list:
			self.disconnect()

		if len(port_list)<=0:
			enabled=False
			self.b_combo.SetSelection(-1)
			self.b_combo.SetValue("No Serial Ports")

		else:
			disconnect_enabled=True

		self.i_school.Enable(not self.arduino.is_opened())
		self.i_robot.Enable(not self.arduino.is_opened())
		self.b_combo.Enable(enabled)
		self.b_connect.Enable(self.validate_input_m(self.i_school) and
			self.validate_input_m(self.i_robot) and (enabled or disconnect_enabled))

	def on_quit_m(self,event):
		if len(self.prompt_text)>0:
			dialog=wx.MessageDialog(self.frame,self.prompt_text,
				"Confirm",wx.OK|wx.CANCEL|wx.ICON_QUESTION)
			result=dialog.ShowModal()
			dialog.Destroy()

			if result==wx.ID_OK:
				self.disconnect()
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

	def validate_input_m(self,input):
		return len(input.GetValue())>0

	def modify_input_m(self,input):
		value=str(input.GetValue()).lower()
		trans=''.join(chr(c) if chr(c).isalnum() else ' ' for c in range(256))
		value=value.translate(trans)
		value=re.sub('\s+','_',value)

		if len(value)>16:
			value=value[:16]

		input.SetValue(value)

	def create_frame_m(self,title):
		size=(400,240)
		self.frame=wx.Frame(None,title=title,size=size)
		self.frame.SetMinSize(size)
		self.frame.SetMaxSize(size)
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

	def create_timer_m(self):
		self.timer=wx.Timer(self.frame)
		self.frame.Bind(wx.EVT_TIMER,self.on_timer_m)
		self.timer.Start(50)

	def create_gui_m(self):
		self.row_0=wx.BoxSizer(wx.HORIZONTAL)
		self.box.Add(self.row_0,0,wx.ALL,10)

		self.t_school=wx.StaticText(self.panel,-1,"School",size=(64,-1))
		self.row_0.Add(self.t_school,0,wx.ALL,10)

		self.i_school=wx.TextCtrl(self.panel,-1,size=(128,-1))
		self.row_0.Add(self.i_school,0,wx.ALL,10)

		self.row_1=wx.BoxSizer(wx.HORIZONTAL)
		self.box.Add(self.row_1,0,wx.ALL,10)

		self.t_robot=wx.StaticText(self.panel,-1,"Robot",size=(64,-1))
		self.row_1.Add(self.t_robot,0,wx.ALL,10)

		self.i_robot=wx.TextCtrl(self.panel,-1,size=(128,-1))
		self.row_1.Add(self.i_robot,0,wx.ALL,10)

		self.row_2=wx.BoxSizer(wx.HORIZONTAL)
		self.box.Add(self.row_2,0,wx.ALL,10)

		self.b_combo=wx.ComboBox(self.panel,-1,size=(240,-1),style=wx.CB_READONLY)
		self.row_2.Add(self.b_combo,0,wx.ALL,10)

		self.b_connect=wx.Button(self.panel,label="Connect")
		self.row_2.Add(self.b_connect,0,wx.ALL,10)
		self.b_connect.Bind(wx.EVT_BUTTON,self.on_connect_m)
		self.b_connect.Enable(False)

	def show_m(self):
		self.frame.Show()
		self.app.MainLoop()
