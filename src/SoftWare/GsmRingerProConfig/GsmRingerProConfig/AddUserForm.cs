using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace GsmRingerProConfig
{
    public partial class AddUserForm : Form
    {
        SerialPortListener listener;
        VoidString setCardId;
        
        public AddUserForm()
        {
            InitializeComponent();
        }

        private void okBtn_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(cardIdBox.Text) || string.IsNullOrEmpty(userNameBox.Text))
                MessageBox.Show("Необходимо заполнить все поля", "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
            else
                this.DialogResult = DialogResult.OK;
        }

        private void AddUserForm_Load(object sender, EventArgs e)
        {
            setCardId = new VoidString(this.SetCardIdBox);
            listener = (this.Owner as GsmRingerProConfig).Listener;
            listener.CardIdReceived += new DeviceDataReceivedEventHandler(listener_CardIdReceived);
        }

        void listener_CardIdReceived(object sender, StringDataReceivedEventArgs e)
        {
            this.Invoke(setCardId, e.Message);
        }

        private void SetCardIdBox(string cardId)
        {
            cardIdBox.Text = cardId;
        }

        private void AddUserForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            listener.CardIdReceived -= this.listener_CardIdReceived;
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
    }

    delegate void VoidString(string input);
}
