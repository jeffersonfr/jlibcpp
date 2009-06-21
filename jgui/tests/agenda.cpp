#include "agenda.h"
#include "jcalendardialog.h"
#include "jkeyboard.h"
#include "jyesnodialog.h"
#include "jmessagedialog.h"
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

	_list = new jgui::ListBox(_insets.left, _insets.top, _width-_insets.left-_insets.right, 200);

	_list->SetBackgroundVisible(false);
	_list->AddItem("Verificar compromissos");
	_list->AddItem("Adicionar compromisso");
	_list->AddItem("Limpar todos os registros");
	_list->AddItem("Estado da mem\xf3ria");
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

		if (event->GetIndex() == 0) {
				Hide();

				ViewMessages view(db, _x, _y);

				view.Show();

				Show(false);
		} else if (event->GetIndex() == 1) {
			if (db->IsFull() == false) {
				Hide();

				AddMessage add(db, _x, _y);

				add.Show();

				Show(false);
			} else {
				jgui::YesNoDialog dialog("Aviso", "A agenda está cheia. Deseja limpar a agenda ?", _x-50, _y+_height+10);

				dialog.Show();

				if (dialog.GetLastKeyCode() != jgui::JKEY_EXIT && dialog.GetResponse() == 1) {
					db->RemoveAll();
				}
			}
		} else if (event->GetIndex() == 2) {
				jgui::YesNoDialog dialog("Aviso", "Remover todos os registros ?", _x-50, _y+_height+10);

				dialog.Show();

				if (dialog.GetLastKeyCode() != jgui::JKEY_EXIT && dialog.GetResponse() == 1) {
						db->RemoveAll();
				}
		} else if (event->GetIndex() == 3) {
				char tmp[255];

				sprintf(tmp, "Contatos usados : %d/%d", db->GetSize(), db->GetCapacity());

				jgui::MessageDialog dialog("Estado da mem\xf3ria", tmp, _x-50, _y+_height+10);

				dialog.Show();
		}
}

}

int main()
{
	jgui::Font *font = new jgui::Font("./fonts/font.ttf", 0, 28);

	font->SetEncoding("Latin1");

	jgui::Graphics::SetDefaultFont(font);

	magenda::Agenda app(100, 100);

	app.Show();

	return 0;
}
