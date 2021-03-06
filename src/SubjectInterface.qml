import QtQuick 2.3

Rectangle {
    id: mainRect
    width: parent.width
    height: parent.height
    color: "black"
    property string rectTxt
    rectTxt: ""
    property string textColor
    textColor: "yellow"

    Text {
        anchors { horizontalCenter: parent.horizontalCenter}
        y: 400
        color: textColor
        font.pointSize: 104 
        font.bold: true
        text: rectTxt
    }

    Image {
        id: plsCircId
        y: 350
        width: 400
        height: 400
        anchors { horizontalCenter: parent.horizontalCenter}
        source: "images/GreenCircle_Pertv3.png"
        visible: false
    }

    Image {
        id: redCircId
        y: 350
        width: 400
        height: 400
        anchors { horizontalCenter: parent.horizontalCenter}
        source: "images/RedCircle_Pertv1.png"
        visible: false
    }


    function hideGreenCircle(){
        console.debug("Hiding Circle")
        plsCircId.visible = false
    }

    function showGreenCircle(){
        plsCircId.visible = true
    }

    function showRedCircle(){
        redCircId.visible = true
    }

    function hideRedCircle(){
        redCircId.visible = false
    }
    
    function setTextColor(color)
    {
        textColor = color
    }
}

