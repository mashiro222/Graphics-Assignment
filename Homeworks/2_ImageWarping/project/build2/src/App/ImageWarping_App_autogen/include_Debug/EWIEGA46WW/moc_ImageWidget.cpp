/****************************************************************************
** Meta object code from reading C++ file 'ImageWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../src/App/ImageWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ImageWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ImageWidget_t {
    QByteArrayData data[16];
    char stringdata0[121];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ImageWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ImageWidget_t qt_meta_stringdata_ImageWidget = {
    {
QT_MOC_LITERAL(0, 0, 11), // "ImageWidget"
QT_MOC_LITERAL(1, 12, 4), // "Open"
QT_MOC_LITERAL(2, 17, 0), // ""
QT_MOC_LITERAL(3, 18, 4), // "Save"
QT_MOC_LITERAL(4, 23, 6), // "SaveAs"
QT_MOC_LITERAL(5, 30, 6), // "Invert"
QT_MOC_LITERAL(6, 37, 6), // "Mirror"
QT_MOC_LITERAL(7, 44, 10), // "horizontal"
QT_MOC_LITERAL(8, 55, 8), // "vertical"
QT_MOC_LITERAL(9, 64, 8), // "TurnGray"
QT_MOC_LITERAL(10, 73, 7), // "Restore"
QT_MOC_LITERAL(11, 81, 7), // "IDWWarp"
QT_MOC_LITERAL(12, 89, 7), // "RBFWarp"
QT_MOC_LITERAL(13, 97, 5), // "Clear"
QT_MOC_LITERAL(14, 103, 7), // "Fillgap"
QT_MOC_LITERAL(15, 111, 9) // "loadDebug"

    },
    "ImageWidget\0Open\0\0Save\0SaveAs\0Invert\0"
    "Mirror\0horizontal\0vertical\0TurnGray\0"
    "Restore\0IDWWarp\0RBFWarp\0Clear\0Fillgap\0"
    "loadDebug"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ImageWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   84,    2, 0x0a /* Public */,
       3,    0,   85,    2, 0x0a /* Public */,
       4,    0,   86,    2, 0x0a /* Public */,
       5,    0,   87,    2, 0x0a /* Public */,
       6,    2,   88,    2, 0x0a /* Public */,
       6,    1,   93,    2, 0x2a /* Public | MethodCloned */,
       6,    0,   96,    2, 0x2a /* Public | MethodCloned */,
       9,    0,   97,    2, 0x0a /* Public */,
      10,    0,   98,    2, 0x0a /* Public */,
      11,    0,   99,    2, 0x0a /* Public */,
      12,    0,  100,    2, 0x0a /* Public */,
      13,    0,  101,    2, 0x0a /* Public */,
      14,    0,  102,    2, 0x0a /* Public */,
      15,    0,  103,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool, QMetaType::Bool,    7,    8,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ImageWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ImageWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->Open(); break;
        case 1: _t->Save(); break;
        case 2: _t->SaveAs(); break;
        case 3: _t->Invert(); break;
        case 4: _t->Mirror((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 5: _t->Mirror((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->Mirror(); break;
        case 7: _t->TurnGray(); break;
        case 8: _t->Restore(); break;
        case 9: _t->IDWWarp(); break;
        case 10: _t->RBFWarp(); break;
        case 11: _t->Clear(); break;
        case 12: _t->Fillgap(); break;
        case 13: _t->loadDebug(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ImageWidget::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_ImageWidget.data,
    qt_meta_data_ImageWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ImageWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ImageWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ImageWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ImageWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 14;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
