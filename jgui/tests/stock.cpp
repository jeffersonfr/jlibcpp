/***************************************************************************
 *   Copyright (C) 2005 by Jeff Ferr                                       *
 *   root@sat                                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "jguilib.h"
#include "jsocketlib.h"

#include <sstream>

class Stock : public jgui::Frame, public jgui::FrameInputListener {

	private:
			jgui::TextField *acao;
			jgui::Label *ldata,
				*lcotacao,
				*lvariacao,
				*lanterior,
				*labertura,
				*lminimo,
				*lmaximo,
				*lvolume,
				*vdata,
				*vcotacao,
				*vvariacao,
				*vanterior,
				*vabertura,
				*vminimo,
				*vmaximo,
				*vvolume;

	public:
		Stock(int x, int y):
			jgui::Frame("Stock", x, y, 500, 400)
		{
			SetMoveEnabled(true);

			int px = 0,
					py = 0,
					pw = DEFAULT_COMPONENT_WIDTH,
					pr = DEFAULT_COMPONENT_WIDTH,
					ph = DEFAULT_COMPONENT_HEIGHT,
					gap = 5;

			acao = new jgui::TextField(px, py+0*(ph+gap), (pw+pr+gap), ph);
			ldata = new jgui::Label("Data", px, py+1*(ph+gap), pw, ph);
			lcotacao = new jgui::Label("Cotacao", px, py+2*(ph+gap), pw, ph);
			lvariacao = new jgui::Label("Variacao", px, py+3*(ph+gap), pw, ph);
			lanterior = new jgui::Label("Anterior", px, py+4*(ph+gap), pw, ph);
			labertura = new jgui::Label("Abertura", px, py+5*(ph+gap), pw, ph);
			lminimo = new jgui::Label("Minimo", px, py+6*(ph+gap), pw, ph);
			lmaximo = new jgui::Label("Maximo", px, py+7*(ph+gap), pw, ph);
			lvolume = new jgui::Label("Volume", px, py+8*(ph+gap), pw, ph);
			vdata = new jgui::Label("--/--/--", px+(pw+gap), py+1*(ph+gap), pr, ph);
			vcotacao = new jgui::Label("0.00", px+(pw+gap), py+2*(ph+gap), pr, ph);
			vvariacao = new jgui::Label("0.00%", px+(pw+gap), py+3*(ph+gap), pr, ph);
			vanterior = new jgui::Label("0.00", px+(pw+gap), py+4*(ph+gap), pr, ph);
			vabertura = new jgui::Label("0.00", px+(pw+gap), py+5*(ph+gap), pr, ph);
			vminimo = new jgui::Label("0.00", px+(pw+gap), py+6*(ph+gap), pr, ph);
			vmaximo = new jgui::Label("0.00", px+(pw+gap), py+7*(ph+gap), pr, ph);
			vvolume = new jgui::Label("0.00", px+(pw+gap), py+8*(ph+gap), pr, ph);

			Add(acao);
			Add(ldata);
			Add(lcotacao);
			Add(lvariacao);
			Add(lanterior);
			Add(labertura);
			Add(lminimo);
			Add(lmaximo);
			Add(lvolume);
			Add(vdata);
			Add(vcotacao);
			Add(vvariacao);
			Add(vanterior);
			Add(vabertura);
			Add(vminimo);
			Add(vmaximo);
			Add(vvolume);

			acao->RequestFocus();

			Pack();
			
			RegisterInputListener(this);
		}

		virtual ~Stock() 
		{
			RemoveInputListener(this);

			delete acao;
			delete ldata;
			delete lcotacao;
			delete lvariacao;
			delete lanterior;
			delete labertura;
			delete lminimo;
			delete lmaximo;
			delete lvolume;
			delete vdata;
			delete vcotacao;
			delete vvariacao;
			delete vanterior;
			delete vabertura;
			delete vminimo;
			delete vmaximo;
			delete vvolume;
		}

		std::map<std::string, std::string> RequestQuotes(std::string stock)
		{
			std::map<std::string, std::string> quotes;

			std::ostringstream o;

			char receive[4098];
			int length,
					count = 0;

			o << "GET /d/quotes.csv?s=" << acao->GetText() << "&f=snd1t1l1c1p2poghvt HTTP/1.0\r\n\r\n";

			try {
				jsocket::Socket c("download.finance.yahoo.com", 80);

				c.Send((char *)o.str().c_str(), o.str().size());

				do {
					length = (int)c.Receive((receive+count), 4096);

					if (length <= 0) {
						break;
					}

					count = count + length;
				} while (true);
					
				c.Close();
			} catch (jsocket::SocketException &e) {
				std::cerr << "Cannot receive stock quote data" << std::endl;
			}

			receive[count] = '\0';

			jcommon::StringTokenizer lines(std::string(receive), "\r\n\r\n", jcommon::SPLIT_FLAG, false);
			jcommon::StringTokenizer tokens(lines.GetToken(1), ",", jcommon::SPLIT_FLAG, false);

			quotes["nome"] = tokens.GetToken(1);
			quotes["data"] = jcommon::StringUtils::ReplaceString(tokens.GetToken(2), "\"", "");
			quotes["hora"] = tokens.GetToken(3);
			quotes["cotacao"] = tokens.GetToken(4);
			quotes["variacao"] = jcommon::StringUtils::ReplaceString(tokens.GetToken(6), "\"", "");
			quotes["anterior"] = tokens.GetToken(7);
			quotes["abertura"] = tokens.GetToken(8);
			quotes["minimo"] = tokens.GetToken(9);
			quotes["maximo"] = tokens.GetToken(10);
			quotes["volume"] = tokens.GetToken(11);

			return quotes;
		}

		virtual void InputChanged(jgui::KeyEvent *event)
		{
			if (event->GetType() != jgui::JKEY_PRESSED) {
				return;
			}

			if (event->GetSymbol() == jgui::JKEY_ENTER && GetComponentInFocus() == acao) {
				std::map<std::string, std::string> quotes = RequestQuotes(acao->GetText() + ".sa");
			
				vdata->SetText(quotes["data"]);
				// vhora->SetText(quotes["hora"]);
				vcotacao->SetText(quotes["cotacao"]);
				vvariacao->SetText(quotes["variacao"]);
				vanterior->SetText(quotes["anterior"]);
				vabertura->SetText(quotes["abertura"]);
				vminimo->SetText(quotes["minimo"]);
				vmaximo->SetText(quotes["maximo"]);
				vvolume->SetText(quotes["volume"]);

				if (quotes["variacao"].find("-") != std::string::npos) {
					vvariacao->SetForegroundColor(0xf0, 0x00, 0x00, 0xff);
				} else {
					vvariacao->SetForegroundColor(0x00, 0xf0, 0x00, 0xff);
				}
			}
		}

};

int main()
{
	jgui::GFXHandler::GetInstance()->SetDefaultFont(
			new jgui::Font("./fonts/font.ttf", 0, DEFAULT_FONT_SIZE));

	Stock stock(100, 100);

	stock.Show();

	return 0;
}
