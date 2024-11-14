/****************************************************************************
** Meta object code from reading C++ file 'color_enhancement.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../color_enhancement.h"
#include <QtCore/qmetatype.h>
#include <QtCore/qplugin.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'color_enhancement.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.7.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSColorEnhancementENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSColorEnhancementENDCLASS = QtMocHelpers::stringData(
    "ColorEnhancement"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSColorEnhancementENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

Q_CONSTINIT const QMetaObject ColorEnhancement::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSColorEnhancementENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSColorEnhancementENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSColorEnhancementENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ColorEnhancement, std::true_type>
    >,
    nullptr
} };

void ColorEnhancement::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *ColorEnhancement::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ColorEnhancement::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSColorEnhancementENDCLASS.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "PluginInterfaceColorEnhancement"))
        return static_cast< PluginInterfaceColorEnhancement*>(this);
    if (!strcmp(_clname, "agustin.tortolero"))
        return static_cast< PluginInterfaceColorEnhancement*>(this);
    return QObject::qt_metacast(_clname);
}

int ColorEnhancement::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    return _id;
}

#ifdef QT_MOC_EXPORT_PLUGIN_V2
static constexpr unsigned char qt_pluginMetaDataV2_ColorEnhancement[] = {
    0xbf, 
    // "IID"
    0x02,  0x71,  'a',  'g',  'u',  's',  't',  'i', 
    'n',  '.',  't',  'o',  'r',  't',  'o',  'l', 
    'e',  'r',  'o', 
    // "className"
    0x03,  0x70,  'C',  'o',  'l',  'o',  'r',  'E', 
    'n',  'h',  'a',  'n',  'c',  'e',  'm',  'e', 
    'n',  't', 
    0xff, 
};
QT_MOC_EXPORT_PLUGIN_V2(ColorEnhancement, ColorEnhancement, qt_pluginMetaDataV2_ColorEnhancement)
#else
QT_PLUGIN_METADATA_SECTION
Q_CONSTINIT static constexpr unsigned char qt_pluginMetaData_ColorEnhancement[] = {
    'Q', 'T', 'M', 'E', 'T', 'A', 'D', 'A', 'T', 'A', ' ', '!',
    // metadata version, Qt version, architectural requirements
    0, QT_VERSION_MAJOR, QT_VERSION_MINOR, qPluginArchRequirements(),
    0xbf, 
    // "IID"
    0x02,  0x71,  'a',  'g',  'u',  's',  't',  'i', 
    'n',  '.',  't',  'o',  'r',  't',  'o',  'l', 
    'e',  'r',  'o', 
    // "className"
    0x03,  0x70,  'C',  'o',  'l',  'o',  'r',  'E', 
    'n',  'h',  'a',  'n',  'c',  'e',  'm',  'e', 
    'n',  't', 
    0xff, 
};
QT_MOC_EXPORT_PLUGIN(ColorEnhancement, ColorEnhancement)
#endif  // QT_MOC_EXPORT_PLUGIN_V2

QT_WARNING_POP
