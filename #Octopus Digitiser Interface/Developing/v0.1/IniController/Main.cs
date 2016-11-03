using System;
using System.Windows.Forms;
using Nini.Config;
using System.Diagnostics;

namespace IniController
{
    public partial class Main : Form
    {

        private IConfigSource source = null;

        public Main()
        {
            InitializeComponent();
            LoadConfigs();
        }

        private void cmb_mode_SelectedIndexChanged(object sender, EventArgs e)
        {
            int adet = Convert.ToInt32(cmb_mode.SelectedItem);
            
            for (int i = 1; i <= 8; i++)
            {
                GroupBox grp = (GroupBox)Controls.Find(string.Format("channel{0}", i), false)[0];
                grp.Enabled = false;
            }

            for (int i = 1; i <= adet; i++)
            {
                GroupBox grp = (GroupBox)Controls.Find(string.Format("channel{0}", i ), false)[0];
                grp.Enabled = true;

            }
        }

        private void LoadConfigs()
        {
            // Dosya buradan erişilir
            source = new IniConfigSource(@"MultipleRecord.ini");

            //Erişilecek olan dosya başlığı
            IConfig config = source.Configs["Acquisition"];

            //Default Settings
            cmb_mode.Text = config.Get("Mode");
            txt_samplerate.Text = config.Get("SampleRate");
            Depth.Text = config.Get("Depth");
            SegmentSize.Text = config.Get("SegmentSize");
            SegmentCount.Text = config.Get("SegmentCount");
            TriggerHoldOff.Text = config.Get("TriggerHoldOff");
            TriggerTimeout.Text = config.Get("TriggerTimeout");
            TimeStampMode.Text = config.Get("TimeStampMode");
            TimeStampClock.Text = config.Get("TimeStampClock");


            Condition.Text = source.Configs["Trigger1"].Get("Condition");
            Level.Text = source.Configs["Trigger1"].Get("Level");
            cb_source.Text = source.Configs["Trigger1"].Get("Source");


            StartPosition.Text = source.Configs["Application"].Get("StartPosition");
            TransferLength.Text = source.Configs["Application"].Get("TransferLength");
            SegmentStart.Text = source.Configs["Application"].Get("SegmentStart");
            cb_SegmentCount.Text = source.Configs["Application"].Get("SegmentCount");
            SaveFileName.Text = source.Configs["Application"].Get("SaveFileName");
            SaveFileFormat.Text = source.Configs["Application"].Get("SaveFileFormat");

            Range1.Text = source.Configs["Channel1"].Get("Range");
            Coupling1.Text = source.Configs["Channel1"].Get("Coupling");
            Impedance1.Text = source.Configs["Channel1"].Get("Impedance");

            Range2.Text = source.Configs["Channel2"].Get("Range");
            Coupling2.Text = source.Configs["Channel2"].Get("Coupling");
            Impedance2.Text = source.Configs["Channel2"].Get("Impedance");

            Range3.Text = source.Configs["Channel3"].Get("Range");
            Coupling3.Text = source.Configs["Channel3"].Get("Coupling");
            Impedance3.Text = source.Configs["Channel3"].Get("Impedance");

            Range4.Text = source.Configs["Channel4"].Get("Range");
            Coupling4.Text = source.Configs["Channel4"].Get("Coupling");
            Impedance4.Text = source.Configs["Channel4"].Get("Impedance");

            Range5.Text = source.Configs["Channel5"].Get("Range");
            Coupling5.Text = source.Configs["Channel5"].Get("Coupling");
            Impedance5.Text = source.Configs["Channel5"].Get("Impedance");

            Range6.Text = source.Configs["Channel6"].Get("Range");
            Coupling6.Text = source.Configs["Channel6"].Get("Coupling");
            Impedance6.Text = source.Configs["Channel6"].Get("Impedance");

            Range7.Text = source.Configs["Channel7"].Get("Range");
            Coupling7.Text = source.Configs["Channel7"].Get("Coupling");
            Impedance7.Text = source.Configs["Channel7"].Get("Impedance");

            Range8.Text = source.Configs["Channel8"].Get("Range");
            Coupling8.Text = source.Configs["Channel8"].Get("Coupling");
            Impedance8.Text = source.Configs["Channel8"].Get("Impedance");

        }

        private void button1_Click(object sender, EventArgs e)
        {

            // Dosya buradan erişilir
            source = new IniConfigSource(@"MultipleRecord.ini");

            //Erişilecek olan dosya başlığı
            IConfig config = source.Configs["Acquisition"];
            
            //Değiştiriyoruz
            source.Configs["Acquisition"].Set("SampleRate", txt_samplerate.Text);
            //Burada cmb_mode kontrol edilmeli, dual single quad vs yazılmalı
            //source.Configs["Acquisition"].Set("cmb_mode", cmb_mode.Text);
            source.Configs["Acquisition"].Set("Depth", Depth.Text);
            source.Configs["Acquisition"].Set("SegmentSize", SegmentSize.Text);
            source.Configs["Acquisition"].Set("SegmentCount", SegmentCount.Text);
            source.Configs["Acquisition"].Set("TriggerHoldOff", TriggerHoldOff.Text);
            source.Configs["Acquisition"].Set("TriggerTimeout", TriggerTimeout.Text);
            source.Configs["Acquisition"].Set("TimeStampMode", TimeStampMode.Text);
            source.Configs["Acquisition"].Set("TimeStampClock", TimeStampClock.Text);

            source.Configs["Trigger1"].Set("Condition", Condition.Text);
            source.Configs["Trigger1"].Set("Level", Level.Text);
            source.Configs["Trigger1"].Set("Source", cb_source.Text);

            source.Configs["Application"].Set("StartPosition", StartPosition.Text);
            source.Configs["Application"].Set("TransferLength", TransferLength.Text);
            source.Configs["Application"].Set("SegmentStart", SegmentStart.Text);
            source.Configs["Application"].Set("SegmentCount", cb_SegmentCount.Text);
            source.Configs["Application"].Set("SaveFileName", SaveFileName.Text);
            source.Configs["Application"].Set("SaveFileFormat", SaveFileFormat.Text);

            source.Configs["Channel1"].Set("Range", Range1.Text);
            source.Configs["Channel1"].Set("Coupling", Coupling1.Text);
            source.Configs["Channel1"].Set("Impedance", Impedance1.Text);

            source.Configs["Channel2"].Set("Range", Range2.Text);
            source.Configs["Channel2"].Set("Coupling", Coupling2.Text);
            source.Configs["Channel2"].Set("Impedance", Impedance2.Text);

            source.Configs["Channel3"].Set("Range", Range3.Text);
            source.Configs["Channel3"].Set("Coupling", Coupling3.Text);
            source.Configs["Channel3"].Set("Impedance", Impedance3.Text);

            source.Configs["Channel4"].Set("Range", Range4.Text);
            source.Configs["Channel4"].Set("Coupling", Coupling4.Text);
            source.Configs["Channel4"].Set("Impedance", Impedance4.Text);

            source.Configs["Channel5"].Set("Range", Range5.Text);
            source.Configs["Channel5"].Set("Coupling", Coupling5.Text);
            source.Configs["Channel5"].Set("Impedance", Impedance5.Text);

            source.Configs["Channel6"].Set("Range", Range6.Text);
            source.Configs["Channel6"].Set("Coupling", Coupling6.Text);
            source.Configs["Channel6"].Set("Impedance", Impedance6.Text);

            source.Configs["Channel7"].Set("Range", Range7.Text);
            source.Configs["Channel7"].Set("Coupling", Coupling7.Text);
            source.Configs["Channel7"].Set("Impedance", Impedance7.Text);

            source.Configs["Channel8"].Set("Range", Range8.Text);
            source.Configs["Channel8"].Set("Coupling", Coupling8.Text);
            source.Configs["Channel8"].Set("Impedance", Impedance8.Text);

            // Save the INI file
            source.Save();

            //Ekrana bastır,eski değer
            //MessageBox.Show(config.Get("SampleRate").ToString());
            MessageBox.Show("Settings are saved!");
        }

        private void button43_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void button44_Click(object sender, EventArgs e)
        {
            //Some codes should be written here to check someting wrong in setting file!!!

            Process p1 = new Process();
            p1.StartInfo.FileName = "GMR.exe";

            //programa baslamadan önce parametre vermek için
            //p1.StartInfo.Arguments = selected_mail_id.ToString() + " " + ssAdmin.LoginForm.user_id.ToString();

            p1.Start();
        }

        private void button45_Click(object sender, EventArgs e)
        {
            Process.Start("notepad.exe", @"MultipleRecord.ini");
        }

        private void button46_Click(object sender, EventArgs e)
        {
            System.Diagnostics.Process proc = new System.Diagnostics.Process();
            proc.StartInfo.FileName = Application.StartupPath + "\\HWM.pdf";
            try
            {
                proc.StartInfo.UseShellExecute = true;
                proc.Start();
            }
            catch
            {
                MessageBox.Show("File is not exists.");
            }
        }

        private void button47_Click(object sender, EventArgs e)
        {
            /*
            Process pidl = new Process();
            pidl.StartInfo.FileName = "idlde.exe";

            //programa baslamadan önce parametre vermek için
            pidl.StartInfo.Arguments = " -vm=file.sav";
            pidl.Start();
            */
            Process.Start("idlde.exe", "example.pro");
        }
    }
}
