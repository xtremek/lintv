 
#include "lintvrenamedialog.h"

#include <kfileitem.h>
#include <klineedit.h>
#include <klocale.h>

#include <QLabel>
#include <QBoxLayout>

LinTVRenameDialog::LinTVRenameDialog(QWidget *parent, const KFileItem item) 
    : KDialog(parent)
{
    const QSize minSize = minimumSize();
    setMinimumSize(QSize(320, minSize.height()));

    setCaption(i18nc("@title:window", "Rename Item"));
    setButtons(Ok | Cancel);
    setDefaultButton(Ok);

    setButtonGuiItem(Ok, KGuiItem(i18nc("@action:button", "Rename"), "dialog-ok-apply"));

    QWidget* page = new QWidget(this);
    setMainWidget(page);

    QVBoxLayout *topLayout = new QVBoxLayout(page);
    topLayout->setMargin(KDialog::marginHint());

    m_newName = item.name();
    QLabel *editLabel = new QLabel(i18nc("@label:textbox", 
                                        "Rename the item <filename>%1</filename> to:", 
                                          m_newName), page);
    m_lineEdit = new KLineEdit(page);
    QString extension = KMimeType::extractKnownExtension(item.url().prettyUrl());
    if (extension.length() > 0) {
        extension.insert(0, ".");
    }

    int selectionLength = m_newName.length();
    if (extension.length() > 0) {
        selectionLength -= extension.length();
    }

    m_lineEdit->setText(m_newName);
    m_lineEdit->setSelection(0, selectionLength);
    m_lineEdit->setFocus();

    topLayout->addWidget(editLabel);
    topLayout->addWidget(m_lineEdit);
}

LinTVRenameDialog::~LinTVRenameDialog()
{
}

void LinTVRenameDialog::slotButtonClicked(int button)
{
    if (button == Ok) {
        m_newName = m_lineEdit->text();
    }

    KDialog::slotButtonClicked(button);
}