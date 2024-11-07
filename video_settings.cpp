#include "video_settings.h"
#include <QDebug>
#include <QFont>
#include <QStandardPaths>

VideoSettings::VideoSettings(QWidget *parent)
    : QDialog(parent),
    pathEdit(new QLineEdit(this)),
    formatComboBox(new QComboBox(this)),
    okButton(new QPushButton("OK", this)),
    cancelButton(new QPushButton("Cancel", this)),
    selectButton(new QPushButton("Output", this)),
    saveCheckBox(new QCheckBox("Save", this)),
    vectorFilterCheckbox(new QCheckBox("α-Trimmed Vector Median Filter", this)),
    colorEnhancementCheckbox(new QCheckBox("Color Enhancement", this))
{
    // Set default path to the Videos folder
    QString videosPath = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
    pathEdit->setText(videosPath);

    // Setup format options
    formatComboBox->addItems({"AVI", "MKV"});

    // Layout setup
    QVBoxLayout *layout = new QVBoxLayout(this);
    QHBoxLayout *pathLayout = new QHBoxLayout();
    pathLayout->addWidget(pathEdit);
    pathLayout->addWidget(selectButton);

    layout->addWidget(vectorFilterCheckbox);
    layout->addWidget(colorEnhancementCheckbox);
    layout->addWidget(saveCheckBox);
    layout->addLayout(pathLayout);
    layout->addWidget(formatComboBox);
    layout->addWidget(okButton);
    layout->addWidget(cancelButton);

    // Set font size and family
    QFont font("Verdana", 12); // Increase font size by 4 (default is usually 8)
    this->setFont(font);
    pathEdit->setFont(font);
    formatComboBox->setFont(font);
    okButton->setFont(font);
    cancelButton->setFont(font);
    selectButton->setFont(font);
    saveCheckBox->setFont(font);
    vectorFilterCheckbox->setFont(font);
    colorEnhancementCheckbox->setFont(font);

    // Set fixed size of the dialog
    setFixedSize(700, 300);

    // Connect signals and slots
    connect(okButton, &QPushButton::clicked, this, &VideoSettings::acceptDialog);
    connect(cancelButton, &QPushButton::clicked, this, &VideoSettings::reject);
    connect(selectButton, &QPushButton::clicked, this, &VideoSettings::selectPath);
    connect(saveCheckBox, &QCheckBox::toggled, this, &VideoSettings::togglePathEdit);
    togglePathEdit(saveCheckBox->isChecked());
}


void VideoSettings::togglePathEdit(bool checked) {
    pathEdit->setEnabled(checked);
    selectButton->setEnabled(checked);
}

QString VideoSettings::getOutputPath() const {
    qDebug() << "Getting output path:" << pathEdit->text();
    return pathEdit->text();
}

QString VideoSettings::getSelectedFormat() const {
    qDebug() << "Getting selected format:" << formatComboBox->currentText();
    return formatComboBox->currentText();
}

bool VideoSettings::isVectorFilterEnabled() const {
    return vectorFilterCheckbox->isChecked(); // Return the state of the vector filter checkbox
}

bool VideoSettings::isColorEnhancementEnabled() const {
    return colorEnhancementCheckbox->isChecked(); // Return the state of the color enhancement checkbox
}

bool VideoSettings::isSaveEnabled() const {
    return saveCheckBox->isChecked();
}



void VideoSettings::acceptDialog() {
    if (saveCheckBox->isChecked() && pathEdit->text().isEmpty()) {
        // If "Save" is checked, the output path must not be empty
        qDebug() << "Output path is empty!";
        QMessageBox::warning(this, "Path Error", "Please select a valid output path.");
        return;
    }

    // If "Save" is unchecked or the path is valid, accept the dialog
    qDebug() << "Dialog accepted with output path:" << pathEdit->text();
    accept();
}


void VideoSettings::selectPath() {
    QString fileName = QFileDialog::getSaveFileName(this, "Select Output Path", QString(), "Videos (*.avi *.mkv)");
    if (!fileName.isEmpty()) {
        pathEdit->setText(fileName);
        qDebug() << "Selected output path:" << fileName;
    } else {
        qDebug() << "No path selected.";
    }
}

void VideoSettings::applyTheme(const QString &theme)
{
    qDebug() << "Applying theme in VideoSettings:" << theme;

    QString styleSheet;

    if (theme == "Light") {
        styleSheet += "background-color: white; color: black;";

        QString checkboxStyle = "QCheckBox::indicator {"
                                "border: 1px solid black;"
                                "background-color: white;"
                                "width: 20px; height: 20px;"
                                "}"
                                "QCheckBox::indicator:checked {"
                                "background-color: lightgray;"
                                "border: 1px solid black;"
                                "}"
                                "QCheckBox { color: black; }";  // Ensure checkbox text is visible
        styleSheet += checkboxStyle;

        // Button styles for Light theme
        QString buttonStyle = "QPushButton {"
                              "background-color: #f0f0f0;"
                              "color: black;"
                              "border: 1px solid #d0d0d0;"
                              "padding: 5px;"
                              "border-radius: 4px;"
                              "}"
                              "QPushButton:hover {"
                              "background-color: #e0e0e0;"
                              "}"
                              "QPushButton:pressed {"
                              "background-color: #d0d0d0;"
                              "}";
        styleSheet += buttonStyle;

    } else if (theme == "Dark") {
        styleSheet += "background-color: black; color: white;";

        QString checkboxStyle = "QCheckBox::indicator {"
                                "border: 1px solid white;"
                                "background-color: black;"
                                "width: 20px; height: 20px;"
                                "}"
                                "QCheckBox::indicator:checked {"
                                "background-color: darkgray;"
                                "border: 1px solid white;"
                                "}";
        styleSheet += checkboxStyle;

        // Button styles for Dark theme
        QString buttonStyle = "QPushButton {"
                              "background-color: #444;"
                              "color: white;"
                              "border: 1px solid #666;"
                              "padding: 5px;"
                              "border-radius: 4px;"
                              "}"
                              "QPushButton:hover {"
                              "background-color: #555;"
                              "}"
                              "QPushButton:pressed {"
                              "background-color: #333;"
                              "}";
        styleSheet += buttonStyle;

    } else if (theme == "Cyberpunk") {
        styleSheet += "background-color: #1E1E2F; color: #00FF7F;";  // Cyberpunk colors
        styleSheet += "QMenu { background-color: #1a1a1a; color: #00ffcc; }"
                      "QMenu::item { padding: 5px; }"
                      "QMenu::item:selected { background-color: #00ffcc; color: black; }";

        QString checkboxStyle = "QCheckBox::indicator {"
                                "border: 1px solid #00ffcc;"
                                "background-color: #1a1a1a;"
                                "width: 20px; height: 20px;"
                                "}"
                                "QCheckBox::indicator:checked {"
                                "background-color: #00ffcc;"
                                "border: 1px solid #1a1a1a;"
                                "}";
        styleSheet += checkboxStyle;

        // Button styles for Cyberpunk theme
        QString buttonStyle = "QPushButton {"
                              "background-color: #1E1E2F;"
                              "color: #00FF7F;"
                              "border: 1px solid #00FF7F;"
                              "padding: 5px;"
                              "border-radius: 4px;"
                              "}"
                              "QPushButton:hover {"
                              "background-color: #00FF7F;"
                              "color: black;"
                              "}"
                              "QPushButton:pressed {"
                              "background-color: #00cc66;"
                              "}";
        styleSheet += buttonStyle;

        styleSheet += "QLabel { color: #00FF7F; font-size: 16px; }";  // Updated label color
    }

    this->setStyleSheet(styleSheet);
    this->update();
}
