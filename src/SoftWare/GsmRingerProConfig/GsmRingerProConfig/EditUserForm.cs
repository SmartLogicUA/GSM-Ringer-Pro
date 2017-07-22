using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace GsmRingerProConfig
{
    public partial class EditUserForm : Form
    {
        SerialPortListener listener;
        VoidString setIdBox;
        
        public EditUserForm(string CardId, string UserName)
        {
            InitializeComponent();
            cardIdBox.Text = CardId;
            userNameBox.Text = UserName;
        }

        private void EditUserForm_Load(object sender, EventArgs e)
        {
            setIdBox = new VoidString(SetIdBox);
            listener = (this.Owner as GsmRingerProConfig).Listener;
            listener.CardIdReceived += new DeviceDataReceivedEventHandler(listener_CardIdReceived);
        }

        void listener_CardIdReceived(object sender, StringDataReceivedEventArgs e)
        {
            this.Invoke(setIdBox, e.Message);
        }

        void SetIdBox(string cardId)
        {
            cardIdBox.Text = cardId;
        }

        public string CardId
        {
            get
            {
                return cardIdBox.Text;
            }
        }

        public string UserName
        {
            get
            {
                return userNameBox.Text;
            }
        }

        private void EditUserForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            listener.CardIdReceived -= listener_CardIdReceived;
        }

        private void okBtn_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(cardIdBox.Text) || string.IsNullOrEmpty(userNameBox.Text))
                MessageBox.Show("Необходимо заполнить все поля", "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
            else
                this.DialogResult = DialogResult.OK;
        }
    }
}
