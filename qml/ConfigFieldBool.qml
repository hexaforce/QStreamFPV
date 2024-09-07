import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

Column{
    Material.theme: Material.Dark
  property string key: ""
  CheckBox {
    text: key
    checked: settings.loadBool(key)
    onCheckedChanged: {
      settings.saveBool(key, checked)
    }
  }
}
