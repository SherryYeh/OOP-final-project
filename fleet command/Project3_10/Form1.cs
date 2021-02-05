using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Project310
{
    
    public partial class Form1 : Form
    {
        System.Media.SoundPlayer  player = new System.Media.SoundPlayer();

        public Form1()
        {
            InitializeComponent();
            player.SoundLocation = "ding.wav";
            //player.Load();
            player.Play();
        }
       
    }
}
