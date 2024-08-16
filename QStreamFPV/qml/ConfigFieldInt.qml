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
    text: settings.loadIntParameter(key).toString()
    onTextChanged: {
      if (text.match(/^\d+$/)) {
        settings.saveIntParameter(key, parseInt(text))
      }
    }
  }
}
