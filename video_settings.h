#ifndef VIDEO_SETTINGS_H
#define VIDEO_SETTINGS_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QCheckBox>

class VideoSettings : public QDialog {
    Q_OBJECT

public:
    VideoSettings(QWidget *parent = nullptr);

    QString getOutputPath() const;
    QString getSelectedFormat() const;
    bool isVectorFilterEnabled() const;
    bool isColorEnhancementEnabled() const;

    bool isSaveEnabled() const; // Method to check save state

private:
    QLineEdit *pathEdit;
    QComboBox *formatComboBox;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QPushButton *selectButton; // New button for selecting path

    QCheckBox *vectorFilterCheckbox; // Add this line
    QCheckBox *colorEnhancementCheckbox; // Add this line
    QCheckBox *saveCheckBox; // Checkbox for saving


public slots:
    void applyTheme(const QString &theme);

private slots:
    void acceptDialog();
    void selectPath(); // Slot for path selection
    void togglePathEdit(bool checked); // Slot to handle the toggle
};

#endif // VIDEO_SETTINGS_H
