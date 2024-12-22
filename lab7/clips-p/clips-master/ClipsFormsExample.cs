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
        private CLIPSNET.Environment clips = new CLIPSNET.Environment();
        /// <summary>
        /// Распознаватель речи
        /// </summary>
        //private Microsoft.Speech.Synthesis.SpeechSynthesizer synth;
        
        /// <summary>
        /// Распознавалка
        /// </summary>
        //private Microsoft.Speech.Recognition.SpeechRecognitionEngine recogn;
        private List<string> ruleConfidenceFacts = new List<string>();
        public ClipsFormsExample()
        {
            InitializeComponent();
            typeDropdown.SelectedIndex = 0;
        }

        protected override void OnLoad(EventArgs e)
        {
            base.OnLoad(e);
        }

        private void HandleResponse()
        {
            //  Вытаскиаваем факт из ЭС
            String evalStr = "(find-fact ((?f ioproxy)) TRUE)";
            FactAddressValue fv = (FactAddressValue)((MultifieldValue)clips.Eval(evalStr))[0];

            MultifieldValue damf = (MultifieldValue)fv["messages"];
            MultifieldValue vamf = (MultifieldValue)fv["answers"];

            outputBox.Text += "---------------------------------------------------------'\r\nНовая итерация : " + System.Environment.NewLine;
            for (int i = 0; i < damf.Count; i++)
            {
                LexemeValue da = (LexemeValue)damf[i];
                byte[] bytes = Encoding.Default.GetBytes(da.Value);
                string message = Encoding.UTF8.GetString(bytes);
                //synth.SpeakAsync(message);
                outputBox.Text += message + System.Environment.NewLine;
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

        private void nextBtn_Click(object sender, EventArgs e)
        {
            print_facts();
            clips.Run();
            print_facts();
            HandleResponse();
        }

        private void resetBtn_Click(object sender, EventArgs e)
        {
            outputBox.Text = "Выполнены команды Clear и Reset." + System.Environment.NewLine;
            //  Здесь сохранение в файл, и потом инициализация через него
            clips.Clear();

            //  Так тоже можно - без промежуточного вывода в файл
            clips.LoadFromString(codeBox.Text);

            clips.Reset();
        }

        String map_fact_number_to_group(String fact)
        {
            int number = int.Parse(fact.Substring(1));
            if (number < 200)
            {
                return "type";
            } else if (number < 300)
            {
                return "requirement";
            } else if (number < 400)
            {
                return "platform";
            } else if (number < 500)
            {
                return "language";
            } else if (number < 600)
            {
                return "framework";
            } else
            {
                return "language_property";
            }
        }
        private void openFile_Click(object sender, EventArgs e)
        {
            if (clipsOpenFileDialog.ShowDialog() == DialogResult.OK)
            {
                Dictionary<String, String> facts = new Dictionary<String, String>();
                int ruleNum = 0;
                StringBuilder database = new StringBuilder();
                database.Append(System.IO.File.ReadAllText("../../common_rules_2.0.txt"));
                foreach (String line in System.IO.File.ReadAllLines(clipsOpenFileDialog.FileName))
                {
                    if (line.Contains(':'))
                    {
                        String[] parts = line.Split(':');
                        facts[parts[0].Trim()] = parts[1].Trim();
                        
                    }
                    else if (line.Contains('='))
                    {
                        String[] ruleConf = line.Split(';');
                        String[] premiseResult = ruleConf[0].Split('=');
                        String resultGroup = map_fact_number_to_group(premiseResult[1].Trim());
                        double ruleConfidence = 0.9;
                        
                        if (ruleConf.Length > 1 && ruleConf[1].Trim().Length > 0)
                        {
                            ruleConfidence = double.Parse(ruleConf[1].Trim(), NumberStyles.Any, CultureInfo.InvariantCulture);
                        }
                        int salience = int.Parse(premiseResult[1].Trim()[1].ToString());
                        if (salience == 6 || salience < 3)
                        {
                            salience = 0;
                        }
                        StringBuilder defrule = new StringBuilder($"(defrule rule{ruleNum}_preprocess\r\n");
                        /*defrule.Append($"\t(declare (salience {salience * 2 + 1}))\r\n");
                        int i = 1;
                        foreach (String premise in premiseResult[0].Split(','))
                        {
                            String premiseGroup = map_fact_number_to_group(premise.Trim());
                            defrule.Append($"\t(fact_with_confidence (fact \"{facts[premise.Trim()]}\") (confidence ?f{i}_cf))\r\n");
                            i++;
                        }
                        defrule.Append($"\t?f <- (fact_with_confidence (fact \"{facts[premiseResult[1].Trim()]}\") (confidence ?cf))\r\n");
                        defrule.Append($"=>\r\n");
                        defrule.Append($"\t(retract ?f)\r\n");
                        defrule.Append($"\t(run)\r\n)\r\n");
                        database.Append(defrule);*/

                        defrule = new StringBuilder($"(defrule rule{ruleNum}\r\n");
                        defrule.Append($"\t(declare (salience {7 - salience}))\r\n");
                        int i = 1;
                        foreach (String premise in premiseResult[0].Split(','))
                        {
                            String premiseGroup = map_fact_number_to_group(premise.Trim());
                            defrule.Append($"\t(fact_with_confidence (fact \"{facts[premise.Trim()]}\") (confidence ?f{i}_cf))\r\n");
                            i++;
                        }
                        defrule.Append($"=>\r\n");
                        defrule.Append($"\t(bind ?min_cf (min 1.0");
                        i--;
                        while (i >= 1)
                        {
                            defrule.Append($" ?f{i}_cf");
                            i--;
                        }
                        defrule.Append("))\r\n");

                        defrule.Append($"\t(bind ?min_cf (max 0 ?min_cf))\r\n");
                        /*defrule.Append($"\t(bind ?min_pos 10)\r\n");
                        defrule.Append($"\t(bind ?max_neg -10)\r\n");
                        int j = 1;
                        while (j < i)
                        {
                            defrule.Append($"\t(if (> ?f{j}_cf 0)\r\n");
                            defrule.Append($"\tthen\r\n");
                            defrule.Append($"\t(bind ?min_pos (min ?min_pos ?f{j}_cf))\r\n");
                            defrule.Append($"\telse\r\n");
                            defrule.Append($"\t(bind ?max_neg (max ?max_neg ?f{j}_cf))\r\n");
                            defrule.Append($"\t)\r\n");
                            j++;
                        }
                        defrule.Append($"\t(bind ?facts_comb 0)\r\n");

                        defrule.Append($"\t(if (eq ?max_neg -10)\r\n");
                        defrule.Append($"\tthen\r\n");
                        defrule.Append($"\t(bind ?facts_comb (+ 0 ?facts_comb))\r\n");
                        defrule.Append($"\telse\r\n");
                        defrule.Append($"\t(bind ?facts_comb (+ ?max_neg ?facts_comb))\r\n");
                        defrule.Append($"\t)\r\n");

                        defrule.Append($"\t(if (eq ?min_pos 10)\r\n");
                        defrule.Append($"\tthen\r\n");
                        defrule.Append($"\t(bind ?facts_comb (+ 0 ?facts_comb))\r\n");
                        defrule.Append($"\telse\r\n");
                        defrule.Append($"\t(bind ?facts_comb (+ ?min_pos ?facts_comb))\r\n");
                        defrule.Append($"\t)\r\n");*/

                        var rulCf = ruleConfidence.ToString(CultureInfo.InvariantCulture);
                        /*defrule.Append($"\t(bind ?res_сf (/ (* ?facts_comb {rulCf}) (- (+ ?facts_comb {rulCf}) (* ?facts_comb {rulCf}))))\r\n");*/
                        defrule.Append($"\t(bind ?res_сf (* ?min_cf {rulCf}))\r\n");
                        defrule.Append($"\t(assert (fact_with_confidence (fact \"{facts[premiseResult[1].Trim()]}\") (confidence ?res_сf)))\r\n");
                        defrule.Append($"\t(assert (sendmessagehalt \"Выведено: {facts[premiseResult[1].Trim()]} с уверенностью \" (str-cat ?res_сf)))\r\n)\r\n");
                        database.Append(defrule);

                        ruleNum += 1;
                    }
                }
                this.typeDropdown.Items.Clear();
                this.typeDropdown.Items.AddRange(facts.Values.ToArray());
                typeDropdown.SelectedIndex = 0;
                codeBox.Text = database.ToString();
                //codeBox.Text = System.IO.File.ReadAllText(clipsOpenFileDialog.FileName);
                Text = "Экспертная система \"Музыка\" – " + clipsOpenFileDialog.FileName;


            }
        }
        private void typePick_Click(object sender, EventArgs e)
        {
            string type = typeDropdown.GetItemText(typeDropdown.Items[typeDropdown.SelectedIndex]);
            clips.Eval($"(assert (add-fact (fact \"{type}\") (confidence {typeConfidenceTB.Text})))");
            clips.Run();
            print_facts();
            clips.Run();
            print_facts();
            HandleResponse();
        }

    }
}
