#include "GtkAddMagnetLinkWindow.hpp"
#include <gtkmm/filechooserdialog.h>
#include "GtkMainWindow.hpp"
#include <Application.hpp>
#include <gtkmm/button.h>
#include <gtkmm/hvseparator.h>
#include <gtkmm/stock.h>
#include <glibmm.h>

GtkMainWindow::GtkMainWindow() :
	m_core(Application::getSingleton()->getCore())
{
	this->set_position(Gtk::WIN_POS_CENTER);
	this->set_default_size(800, 500);

	header = Gtk::manage(new Gtk::HeaderBar());
	header->set_title("gTorrent");
	header->set_show_close_button(true);

	// This needs to be refactored

	Gtk::Button     *btn_add_torrent = Gtk::manage(new Gtk::Button());
	Gtk::Button     *btn_add_link    = Gtk::manage(new Gtk::Button());
	Gtk::Button     *btn_pause       = Gtk::manage(new Gtk::Button());
	Gtk::VSeparator *separator       = Gtk::manage(new Gtk::VSeparator());
	m_treeview      = Gtk::manage(new GtkTorrentTreeView());
	m_treemenu      = Gtk::manage(new Gtk::Menu());

	btn_add_torrent->set_image_from_icon_name("gtk-add");
	btn_add_link->set_image_from_icon_name("gtk-paste");
	btn_pause->set_image_from_icon_name("gtk-media-pause");

	btn_add_torrent->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onAddBtnClicked));
	btn_add_link->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onAddMagnetBtnClicked));
	m_treeview->signal_button_press_event().connect(sigc::mem_fun(*this, &GtkMainWindow::onTreeViewClicked));      

	header->add(*btn_add_torrent);
	header->add(*btn_add_link);
	header->add(*separator);
	header->add(*btn_pause);

	this->add(*m_treeview);
	this->set_titlebar(*header);

	Glib::signal_timeout().connect(sigc::mem_fun(*this, &GtkMainWindow::onSecTick), 10);

	this->signal_delete_event().connect(sigc::mem_fun(*this, &GtkMainWindow::onDestroy));

	this->show_all();
}

bool GtkMainWindow::onSecTick()
{
	m_treeview->updateCells();
	return true;
}

bool GtkMainWindow::onTreeViewClicked(GdkEventButton *ev)
{
	if (ev->type == GDK_BUTTON_PRESS && ev->button == 3) // Right mouse button
	{
		m_treemenu->popup(ev->button, ev->time);
		return true;
	}
	// TODO read columns, display ticks depending on whether they are enabled or not
	// -- nyanpasu
	
	return false;
}

void GtkMainWindow::onAddBtnClicked()
{
	Gtk::FileChooserDialog fc("Browse for torrent file", Gtk::FILE_CHOOSER_ACTION_OPEN);
	fc.set_select_multiple();
	fc.set_transient_for(*this);
	fc.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
	fc.add_button("Select", Gtk::RESPONSE_OK);

	Glib::RefPtr<Gtk::FileFilter> filter_t = Gtk::FileFilter::create();
	filter_t->set_name("Torrent Files");
	filter_t->add_mime_type("application/x-bittorrent");
	fc.add_filter(filter_t);

	int result = fc.run();

	switch (result) {
		case Gtk::RESPONSE_OK:
			for (auto &f : fc.get_filenames())
			{
				shared_ptr<Torrent> t = m_core->addTorrent(f.c_str());
				m_treeview->addCell(t);
			}
			break;
	}
}

void GtkMainWindow::onAddMagnetBtnClicked()
{
	GtkAddMagnetLinkWindow d;
	d.set_transient_for(*this);
	int r = d.run();

	switch (r) {
		case Gtk::RESPONSE_OK:
			shared_ptr<Torrent> t = m_core->addTorrent(d.getMagnetURL());
			m_treeview->addCell(t);
			break;
	}
}

bool GtkMainWindow::onDestroy(GdkEventAny *event)
{
	m_core->shutdown();
	return false;
}
