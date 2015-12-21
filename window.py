import arduino
from arduino import millis
from enum import Enum
import packetize
import serial
import time
import wx

class state_t(Enum):
	DISCONNECTED=0,
	CONNECTING=1,
	UNSETDTR=2,
	FLUSHING=3,
	SETDTR=4,
	CONNECTED=5

class window_t:
	def __init__(self):
		self.arduino=arduino.arduino_t()
		self.parser=packetize.parser_t()
		self.state_m=state_t.DISCONNECTED
		self.state_timer_m=0
		self.arduino_port=""
		self.prompt_text=""

		self.app=wx.App(redirect=True)
		self.create_frame_m("Rasa Iterum")

		self.create_status_m()
		self.set_status('Disconnected.')

		self.create_menu_bar_m()
		self.create_file_menu_m()
		self.create_help_menu_m()

		self.create_panel_box_m()

		self.timer=wx.Timer(self.frame)
		self.frame.Bind(wx.EVT_TIMER,self.on_timer_m)
		self.timer.Start(50)

		self.b_text=wx.StaticText(self.panel,-1,"Serial Port")
		self.box.Add(self.b_text,0,wx.ALL,10)
		self.b_text.SetFont(wx.Font(14,wx.SWISS,wx.NORMAL,wx.BOLD))
		self.b_text.SetSize(self.b_text.GetBestSize())

		self.b_combo=wx.ComboBox(self.panel,-1,style=wx.CB_READONLY)
		self.box.Add(self.b_combo,0,wx.ALL,10)

		self.b_connect=wx.Button(self.panel,label="Connect")
		self.box.Add(self.b_connect,0,wx.ALL,10)
		self.b_connect.Bind(wx.EVT_BUTTON,self.on_connect_m)
		self.b_connect.Enable(False)

		self.build_serial_list()

		self.show_m()

	def set_status(self,text):
		self.statusbar.SetStatusText(text)

	def disconnect(self):
		self.arduino.close()
		self.parser.reset()
		self.state_m=state_t.DISCONNECTED
		self.arduino_port=""
		self.prompt_text=""
		self.build_serial_list()
		self.b_connect.SetLabel("Connect")
		self.set_status('Disconnected.')

	def on_connect_m(self,event):
		if self.arduino.is_opened():
			self.disconnect()

		else:
			if self.b_combo.GetCurrentSelection()>=0:
				self.b_connect.SetLabel("Disconnect")
				self.b_combo.Enable(False)

				try:
					self.state_m=state_t.CONNECTING
					self.arduino_port=self.b_combo.GetValue()
					self.prompt_text="Serial port communication in progress, really quit?"
				except Exception as error:
					self.set_status(str(error))

	def on_timer_m(self,event):
		self.build_serial_list()

		if self.b_combo.GetCurrentSelection()<0:
			self.disconnect()
			self.b_combo.Enable(False)
			self.b_connect.Enable(False)
			return

		if self.state_m==state_t.DISCONNECTED:
			self.b_combo.Enable(self.b_combo.GetCurrentSelection()>=0)
			self.b_connect.Enable(self.b_combo.GetCurrentSelection()>=0)

		if self.state_m==state_t.CONNECTING:
			if len(self.arduino_port)>0:
				self.b_connect.SetLabel("Disconnect")
				self.arduino.serial=serial.Serial()
				self.arduino.serial.port=self.arduino_port
				self.old_arduino_port=self.arduino_port
				self.arduino.serial.baudrate=57600

				try:
					self.arduino.serial.open()
				except:
					self.disconnect()

				if not self.arduino.is_opened():
					self.disconnect()
					self.set_status('Could not open Arduino on serial port "'+self.old_arduino_port+'".')

				if self.arduino.is_opened():
					self.set_status('Connecting to "'+self.arduino.name()+'".')
					self.state_m=state_t.UNSETDTR
					self.arduino.serial.setDTR(False)
					self.state_timer_m=millis()+2000

		if self.state_m==state_t.UNSETDTR:
			if millis()>self.state_timer_m:
				self.arduino.serial.flushInput()
				self.arduino.serial.setDTR(True)
				self.state_m=state_t.FLUSHING
				self.state_timer_m=millis()+2000

		if self.state_m==state_t.FLUSHING:
			self.set_status('Connecting to "'+self.arduino.name()+'"..')

			if millis()>self.state_timer_m:
				self.state_m=state_t.SETDTR
				self.state_timer_m=millis()+2000

		if self.state_m==state_t.SETDTR:
			self.set_status('Connecting to "'+self.arduino.name()+'"...')

			if millis()>self.state_timer_m:
				self.state_m=state_t.CONNECTED
				self.set_status('Handshaking with "'+self.arduino.name()+'".')

				check_send_timer=millis()
				check_timer=millis()+2000
				checked=False

				while self.arduino.is_opened() and millis()<=check_timer:
					if millis()>check_send_timer:
						packetize.send_packet('{"s":{}}',self.arduino)
						check_send_timer=millis()+100

					sensors=self.parser.parse(self.arduino)

					if sensors and 'p' in sensors:
						checked=True
						packetize.send_packet('{"c":{}}',self.arduino)
						self.set_status('Connected to an '+str(sensors['p'])+' on "'+self.arduino.name()+'".')
						break

				if not checked:
					self.disconnect()
					self.set_status('Invalid handshake from "'+self.arduino.name()+'".')

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

	def build_serial_list(self):
		self.b_combo.Clear()

		port_list=self.arduino.list()

		for port in port_list:
			self.b_combo.Append(port)

		if len(port_list)>0:
			self.b_combo.SetSelection(0)
		else:
			self.b_combo.SetSelection(-1)
			self.b_combo.SetValue("No Serial Ports")

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