#include "agenda.h"
#include "jcalendardialog.h"
#include "jkeyboard.h"
#include "jyesnodialogbox.h"
#include "jmessagedialogbox.h"
#include "viewmessages.h"
#include "addmessage.h"
#include "agendadb.h"

namespace magenda {

Agenda::Agenda(int x, int y):
   	jgui::Frame("Tarefas", x, y, 600, 420)
{
	started = true;

	db = new AgendaDB("./config/agenda.xml");

	db->Load();

	_list = new jgui::ListBox(_insets.left, _insets.top, GetWidth()-_insets.left-_insets.right, 230);

	_list->SetBackgroundVisible(false);
	_list->AddTextItem("Verificar compromissos");
	_list->AddTextItem("Adicionar compromisso");
	_list->AddTextItem("Limpar todos os registros");
	_list->AddTextItem("Estado da mem\xf3ria");
	_list->RegisterSelectListener(this);

	Add(_list);

	_list->RequestFocus();

	Pack();
}

Agenda::~Agenda() 
{
		jthread::AutoLock lock(&agenda_mutex);

		if (db != NULL) {
			db->Save();

			delete db;
			db = NULL;
		}

		if (_list != NULL) {
			delete _list;
			_list = NULL;
		}
}

void Agenda::ItemSelected(jgui::SelectEvent *event)
{
		jthread::AutoLock lock(&agenda_mutex);

		if (_list->GetItem(0) == event->GetItem()) {
				Hide();

				ViewMessages view(db, GetX(), GetY());

				view.Show();

				Show(false);
		} else if (_list->GetItem(1) == event->GetItem()) {
			if (db->IsFull() == false) {
				Hide();

				AddMessage add(db, GetX(), GetY());

				add.Show();

				Show(false);
			} else {
				jgui::YesNoDialogBox dialog("Aviso", "A agenda está cheia. Deseja limpar a agenda ?", GetX()-50, GetY()+GetHeight()+10);

				dialog.Show();

				if (dialog.GetLastKeyCode() != jgui::JKEY_EXIT && dialog.GetResponse() == 1) {
					db->RemoveAll();
				}
			}
		} else if (_list->GetItem(2) == event->GetItem()) {
				jgui::YesNoDialogBox dialog("Aviso", "Remover todos os registros ?", GetX()-50, GetY()+GetHeight()+10);

				dialog.Show();

				if (dialog.GetLastKeyCode() != jgui::JKEY_EXIT && dialog.GetResponse() == 1) {
						db->RemoveAll();
				}
		} else if (_list->GetItem(3) == event->GetItem()) {
				char tmp[255];

				sprintf(tmp, "Contatos usados : %d/%d", db->GetSize(), db->GetCapacity());

				jgui::MessageDialogBox dialog("Estado da mem\xf3ria", tmp, GetX()-50, GetY()+GetHeight()+10);

				dialog.Show();
		}
}

}

int main()
{
	jgui::Font *font = new jgui::Font("./fonts/font.ttf", 0, 20);

	font->SetEncoding("Latin1");

	jgui::Graphics::SetDefaultFont(font);

	magenda::Agenda app(100, 100);

	app.Show();

	return 0;
}
