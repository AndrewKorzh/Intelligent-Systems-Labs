using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

using CLIPSNET;
using static System.Windows.Forms.VisualStyles.VisualStyleElement;

namespace ClipsFormsExample
{
    public partial class ClipsFormsExample : Form
    {
        private bool isTargetFound;
        private CLIPSNET.Environment clips = new CLIPSNET.Environment();
        private List<string> ruleConfidenceFacts = new List<string>();

        public ClipsFormsExample()
        {
            InitializeComponent();
            factDropdown.SelectedIndex = 0;
            targetDropdown.SelectedIndex = 0;
        }

        protected override void OnLoad(EventArgs e)
        {
            base.OnLoad(e);
        }

        private void HandleResponse()
        {
            checkTarget();

            String evalStr = "(find-fact ((?f ioproxy)) TRUE)";
            FactAddressValue fv = (FactAddressValue)((MultifieldValue)clips.Eval(evalStr))[0];

            MultifieldValue damf = (MultifieldValue)fv["messages"];
            MultifieldValue vamf = (MultifieldValue)fv["answers"];
            if (isTargetFound)
            {
                outputBox.Text += "---------------------------------------------------------'\r" + System.Environment.NewLine;
            }
            else
            {
                outputBox.Text += "---------------------------------------------------------'\r\nНовая итерация : " + System.Environment.NewLine;

            }
            for (int i = 0; i < damf.Count; i++)
            {
                LexemeValue da = (LexemeValue)damf[i];
                byte[] bytes = Encoding.Default.GetBytes(da.Value);
                string message = Encoding.UTF8.GetString(bytes);
                outputBox.Text += message + System.Environment.NewLine;
            }
            if (isTargetFound)
            {
                outputBox.Text += "Target found" + System.Environment.NewLine;
            }
            outputBox.SelectionStart = outputBox.TextLength;
            outputBox.ScrollToCaret();
            if (vamf.Count == 0)
                clips.Eval("(assert (clearmessage))");
        }

        private void print_facts()
        {
            foreach (var fact in clips.GetFactList())
            {
                StringBuilder factData = new StringBuilder($"({fact.RelationName} ");
                fact.GetSlotValues().ForEach(v => { factData.Append($"({v.SlotName} {v.Contents})"); });
                factData.Append(')');
                Console.WriteLine(factData.ToString());
            }
            Console.WriteLine("------------------------------------------------------------------");
        }
        private void checkTarget()
        {
            string target = $"\"{targetDropdown.GetItemText(targetDropdown.Items[targetDropdown.SelectedIndex])}\"";
            foreach (var fact in clips.GetFactList())
            {
                if (fact.GetSlotValues().ToArray()[0].Contents == target)
                {
                    Console.WriteLine("target");
                    isTargetFound = true;
                }
            }
            Console.WriteLine("-------------------check-----------------------------------------------");
        }

        private void nextBtn_Click(object sender, EventArgs e)
        {
            if (!isTargetFound)
            {
                print_facts();
                clips.Run();
                print_facts();
            }
            HandleResponse();

        }

        private void resetBtn_Click(object sender, EventArgs e)
        {
            outputBox.Text = "Выполнены команды Clear и Reset." + System.Environment.NewLine;
            clips.Clear();
            isTargetFound = false;
            clips.LoadFromString(codeBox.Text);
            clips.Reset();
        }


        private void openFile_Click(object sender, EventArgs e)
        {
            if (clipsOpenFileDialog.ShowDialog() == DialogResult.OK)
            {
                isTargetFound = false;
                Dictionary<String, String> facts = new Dictionary<String, String>();
                int ruleNum = 0;
                StringBuilder database = new StringBuilder();
                database.Append(System.IO.File.ReadAllText("../../common_rules.txt"));

                factDropdown.Items.Clear();
                targetDropdown.Items.Clear();


                foreach (String line in System.IO.File.ReadAllLines(clipsOpenFileDialog.FileName))
                {
                    if (line.Contains(':'))
                    {
                        String[] parts = line.Split(':');
                        facts[parts[0].Trim()] = parts[1].Trim();
                        factDropdown.Items.Add(parts[1].Trim());
                    }
                    else if(line.Contains('#'))
                    {
                        String[] parts = line.Split('#');
                        targetDropdown.Items.Add(parts[1]);
                    }
                    else if (line.Contains('='))
                    {
                        String[] ruleConf = line.Split(';');
                        String[] premiseResult = ruleConf[0].Split('=');



                        StringBuilder defrule = new StringBuilder($"(defrule rule{ruleNum}\r\n");
                        int i = 1;
                        foreach (String premise in premiseResult[0].Split(','))
                        {
                            defrule.Append($"\t(fact (name \"{facts[premise.Trim()]}\"))\r\n");
                            i++;
                        }
                        defrule.Append($"=>\r\n");
                        defrule.Append($"\t(assert (fact (name \"{facts[premiseResult[1].Trim()]}\")))\r\n");
                        defrule.Append($"\t(assert (sendmessagehalt \"Выведено: {facts[premiseResult[1].Trim()]}\"))\r\n)\r\n");
                        database.Append(defrule);

                        ruleNum += 1;
                    }
                }
                codeBox.Text = database.ToString();
                Text = "Экспертная система \"Marvel\" – " + clipsOpenFileDialog.FileName;

                clips.Clear();
                clips.LoadFromString(codeBox.Text);
                clips.Reset();
                factDropdown.SelectedItem = factDropdown.Items[0];
                targetDropdown.SelectedItem = targetDropdown.Items[0];

            }
        }

        private void fillComboBox()
        {
            factDropdown.Items.Clear();

            foreach (var fact in clips.GetFactList())
            {
                fact.GetSlotValues().ForEach(v => { factDropdown.Items.Add(v.Contents); });
                //typeDropdown.Items.Add(fact.GetSlotValues().ToArray()[0].Contents);
            }


        }
        private void typePick_Click(object sender, EventArgs e)
        {
            string type = factDropdown.GetItemText(factDropdown.Items[factDropdown.SelectedIndex]);
            clips.Eval($"(assert (add-fact (name \"{type}\")))");
            clips.Run();
            print_facts();
            clips.Run();
            print_facts();
            HandleResponse();
        }

        private void ClipsFormsExample_Load(object sender, EventArgs e)
        {

        }

        private void splitContainer1_Panel2_Paint(object sender, PaintEventArgs e)
        {

        }

        private void outputBox_TextChanged(object sender, EventArgs e)
        {

        }
    }
}
