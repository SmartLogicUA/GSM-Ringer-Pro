using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace GsmRingerProConfig
{
    public partial class CardAdminForm : Form
    {
        Dictionary<string, string> recs;
        AddUserForm addUser;
        EditUserForm editUser;

        public CardAdminForm(Dictionary<string, string> Records)
        {
            InitializeComponent();
            recs = Records;
        }

        private void CardAdminForm_Load(object sender, EventArgs e)
        {
            //dataGridView1.RowCount = recs.Count;
            foreach (System.Collections.Generic.KeyValuePair<string, string> rec in recs)
            {
                dataGridView1.Rows.Add(rec.Key, rec.Value);
            }
        }

        public DataGridView Grid
        {
            get
            {
                return dataGridView1;
            }
        }

        private void addUserBtn_Click(object sender, EventArgs e)
        {
            addUser = new AddUserForm();
            if (addUser.ShowDialog(this.Owner) == DialogResult.OK)
            {
                bool contains = false;
                foreach (DataGridViewRow row in dataGridView1.Rows)
                {
                    if (addUser.CardId == (row.Cells[0].Value as string))
                        contains = true;
                }
                
                if (contains)
                {
                    MessageBox.Show("Пользователь с таким идентификатором уже присутствует", "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
                else
                {
                    dataGridView1.Rows.Add(addUser.CardId, addUser.UserName);
                }
            }
        }

        private void delUserBtn_Click(object sender, EventArgs e)
        {
            if (dataGridView1.SelectedRows.Count > 0)
            {
                if (MessageBox.Show("Вы действительно хотите удалить выделенных пользователей?", "Удаление пользователей", MessageBoxButtons.OKCancel, MessageBoxIcon.Question) == DialogResult.OK)
                {
                    foreach (DataGridViewRow row in dataGridView1.SelectedRows)
                    {
                        dataGridView1.Rows.Remove(row);
                    }
                }
            }
        }

        private void saveBtn_Click(object sender, EventArgs e)
        {
            recs.Clear();

            if (dataGridView1.Rows.Count > 0)
            {
                foreach (DataGridViewRow row in dataGridView1.Rows)
                {
                    recs.Add((row.Cells[0].Value as string), (row.Cells[1].Value as string));
                }
            }
        }

        private void editBtn_Click(object sender, EventArgs e)
        {
            if (dataGridView1.SelectedRows.Count > 0)
            {
                editUser = new EditUserForm((dataGridView1.SelectedRows[0].Cells[0].Value as string), (dataGridView1.SelectedRows[0].Cells[1].Value as string));
                if (editUser.ShowDialog(this.Owner) == DialogResult.OK)
                {
                    bool contains = false;
                    foreach (DataGridViewRow row in dataGridView1.Rows)
                    {
                        if (dataGridView1.Rows.IndexOf(row) != dataGridView1.Rows.IndexOf(dataGridView1.SelectedRows[0]))
                        {
                            if (editUser.CardId == (row.Cells[0].Value as string))
                                contains = true;
                        }
                    }
                    if (contains)
                        MessageBox.Show("Пользователь с таким идентификатором уже присутствует", "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    else
                    {
                        dataGridView1.SelectedRows[0].Cells[0].Value = editUser.CardId;
                        dataGridView1.SelectedRows[0].Cells[1].Value = editUser.UserName;
                    }
                }
            }
        }
    }

    //public class CardRecord
    //{
    //    string cardId;
    //    string userName;

    //    public CardRecord(string CardId, string UserName)
    //    {
    //        cardId = CardId;
    //        userName = UserName;
    //    }

    //    public string CardId
    //    {
    //        get
    //        {
    //            return cardId;
    //        }

    //        set
    //        {
    //            cardId = value;
    //        }
    //    }

    //    public string UserName
    //    {
    //        get
    //        {
    //            return userName;
    //        }

    //        set
    //        {
    //            userName = value;
    //        }
    //    }
    //}
}
