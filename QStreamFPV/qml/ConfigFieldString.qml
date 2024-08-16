import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

Column{
    Material.theme: Material.Dark
  property string key: ""
  Label {
    text: key
    Layout.preferredWidth: 240
  }
  TextField {
    Layout.preferredHeight: 30
    implicitWidth: 245
    text: settings.loadStringParameter(key)
    onTextChanged: {
      settings.saveStringParameter(key, text)
    }
  }
}
