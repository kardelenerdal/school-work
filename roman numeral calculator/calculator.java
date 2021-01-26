package veli;
import java.awt.event.*; 
import javax.swing.*; 
import java.awt.*; 
public class calculator extends JFrame implements ActionListener{

	// Java program to create a simple calculator 
	// with basic +, -, /, * using java swing elements 

	
		// create a frame 
		static JFrame f; 

		// create a textfield 
		static JTextField l; 

		// store oprerator and operands 
		String s0, s1, s2; 

		// default constrcutor 
		calculator() 
		{ 
			s0 = s1 = s2 = ""; 
		} 

		// main function 
		public static void main(String args[]) 
		{ 
			// create a frame 
			f = new JFrame("calculator"); 

			try { 
				// set look and feel 
				UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName()); 
			} 
			catch (Exception e) { 
				System.err.println(e.getMessage()); 
			} 

			// create a object of class 
			calculator c = new calculator(); 

			// create a textfield 
			l = new JTextField(25); 
			l.setPreferredSize(new Dimension(10,150));
			l.setFont(new Font("Arial", Font.BOLD, 50));
			
			// set the textfield to non editable 
			l.setEditable(false); 

			// create number buttons and some operators 
			JButton b0, b1, b5, b10, beq1, ba, bs, bd, bm, beq, bl; 

			// create number buttons 
			b0 = new JButton("0"); 
			b1 = new JButton("I"); 
			b5 = new JButton("V"); 
			b10 = new JButton("X"); 
			bl = new JButton("L");
			 
			b0.setForeground(Color.BLUE);
			b0.setFont(new Font("Arial", Font.PLAIN, 40));
			b1.setForeground(Color.BLUE);
			b1.setFont(new Font("Arial", Font.PLAIN, 40));
			b5.setForeground(Color.BLUE);
			b5.setFont(new Font("Arial", Font.PLAIN, 40));
			b10.setForeground(Color.BLUE);
			b10.setFont(new Font("Arial", Font.PLAIN, 40));
			bl.setForeground(Color.BLUE);
			bl.setFont(new Font("Arial", Font.PLAIN, 40));
			// equals button 
			beq1 = new JButton("="); 

			// create operator buttons 
			ba = new JButton("+"); 
			bs = new JButton("-"); 
			bd = new JButton("/"); 
			bm = new JButton("*"); 
			beq = new JButton("C"); 
			beq1.setForeground(Color.GREEN);
			beq1.setFont(new Font("Arial", Font.PLAIN, 40));
			ba.setForeground(Color.ORANGE);
			ba.setFont(new Font("Arial", Font.PLAIN, 40));
			bs.setForeground(Color.ORANGE);
			bs.setFont(new Font("Arial", Font.PLAIN, 40));
			bd.setForeground(Color.ORANGE);
			bd.setFont(new Font("Arial", Font.PLAIN, 40));
			bm.setForeground(Color.ORANGE);
			bm.setFont(new Font("Arial", Font.PLAIN, 40));
			beq.setForeground(Color.RED);
			beq.setFont(new Font("Arial", Font.PLAIN, 40));
			
			b0.setPreferredSize(new Dimension(100, 100));
			bm.setPreferredSize(new Dimension(100, 100));
			bd.setPreferredSize(new Dimension(100, 100));
			bs.setPreferredSize(new Dimension(100, 100));
			ba.setPreferredSize(new Dimension(100, 100));
			bl.setPreferredSize(new Dimension(100, 100));
			b5.setPreferredSize(new Dimension(100, 100));
			b1.setPreferredSize(new Dimension(100, 100));
			b10.setPreferredSize(new Dimension(100, 100));
			beq.setPreferredSize(new Dimension(100, 100));
			beq1.setPreferredSize(new Dimension(100, 100));

			// create a panel 
			JPanel p = new JPanel(); 

			// add action listeners 
			bm.addActionListener(c); 
			bd.addActionListener(c); 
			bs.addActionListener(c); 
			ba.addActionListener(c); 
			b5.addActionListener(c); 
			b1.addActionListener(c); 
			b0.addActionListener(c); 
			beq.addActionListener(c); 
			beq1.addActionListener(c); 
			b10.addActionListener(c); 
			bl.addActionListener(c); 

			// add elements to panel 
			p.add(l); 
			p.add(b1); 
			p.add(b5); 
			p.add(b10);
			p.add(bl);
			p.add(b0); 
			p.add(bm); 
			p.add(bd); 
			p.add(ba); 
			p.add(bs); 
			p.add(beq); 
			p.add(beq1);

			// set Background of panel 
			p.setBackground(Color.PINK); 

			// add panel to frame 
			f.add(p); 

			f.setSize(1200, 550); 
			f.setVisible(true);
		} 
		public void actionPerformed(ActionEvent e) {
			String s = e.getActionCommand(); 
			if(s.charAt(0) == 'I' || s.charAt(0) == 'X' || s.charAt(0) == 'V' || s.charAt(0)== 'L') {
				if(!s1.equals("")) {
					s2+=s;
				} else {
					s0+=s;
				}
				l.setText(s0+s1+s2);
			} else if(s.charAt(0) == 'C') {
				s0 = s1 = s2 = "";
				l.setText(s0 + s1 + s2); 
			} else if (s.charAt(0) == '=') {
				int res;
				int i0 = 0;
				int i2 = 0;
				for(int i=0; i<s0.length(); i++) {
					if(i+1 < s0.length()) {
						if(value(s0.charAt(i))>=value(s0.charAt(i+1))) {
							i0+=value(s0.charAt(i));
						} else {
							i0+=value(s0.charAt(i+1))-value(s0.charAt(i));
							i++;
						}
					} else {
						i0+=value(s0.charAt(i));
						i++;
					}
				}
				for(int i=0; i<s2.length(); i++) {
					if(i+1 < s2.length()) {
						if(value(s2.charAt(i))>=value(s2.charAt(i+1))) {
							i2+=value(s2.charAt(i));
						} else {
							i2+=value(s2.charAt(i+1))-value(s2.charAt(i));
							i++;
						}
					} else {
						i2+=value(s2.charAt(i));
						i++;
					}
				}
				if (s1.equals("+")) 
					res = i0 + i2; 
				else if (s1.equals("-")) 
					res = i0 - i2; 
				else if (s1.equals("/")) 
					res = i0 / i2; 
				else
					res = i0 * i2; 
				
				l.setText(s0 + s1 + s2 + "=" + toRoman(res)); 

			} else {
				s1=s;
			}
		}
		
		public int value(char r) { 
	        if (r == 'I') 
	            return 1; 
	        if (r == 'V') 
	            return 5; 
	        if (r == 'X') 
	            return 10; 
	        if (r == 'L') 
	            return 50; 
	        if (r == 'C') 
	            return 100; 
	        if (r == 'D') 
	            return 500; 
	        if (r == 'M') 
	            return 1000; 
	        return -1; 
	    } 
		public static String toRoman(int n) {
		    String[] romanNumerals = { "M",  "CM", "D", "CD", "C", "XC", "L",  "X", "IX", "V", "I" };
		    int[] romanNumeralNums = {  1000, 900, 500,  400 , 100,  90,  50,   10,    9,   5,   1 };
		    String finalRomanNum = "";

		    for (int i = 0; i < romanNumeralNums.length; i ++) {
		            int currentNum = n /romanNumeralNums[i];
		            if (currentNum==0) {
		                continue;
		            }

		            for (int j = 0; j < currentNum; j++) {
		                finalRomanNum +=romanNumerals[i];
		            }

		            n = n%romanNumeralNums[i];
		    }
		    return finalRomanNum;
		}
	} 


