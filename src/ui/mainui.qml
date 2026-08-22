import QtQuick 2.15
import QtQuick.Controls

Window {
    id: root
    width: 320
    height: 480
    visible: true
    minimumWidth: 320
    minimumHeight: 480
    maximumWidth: 320
    maximumHeight: 480

    Rectangle {
            anchors.fill: parent
            color: "#0a0a0c"
    }

    Image {
        id: bgRadialBloom
        anchors.centerIn: parent
        source: "../../assets/bg-radial-bloom.svg"
        fillMode: Image.PreserveAspectFit
    }

    Image {
        id: ambientGlowWarm
        anchors.centerIn: parent
        source: "../../assets/ambient-glow-warm.svg"
    }

    Image {
        id: floorReflection
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        source: "../../assets/floor-reflection.svg"
        anchors.horizontalCenterOffset: 0
    }

    Button {
        id: btnLeft
        width: 34
        height: 40

        padding: 0
        topPadding: 0
        bottomPadding: 0
        leftPadding: 0
        rightPadding: 0

        display: AbstractButton.IconOnly

        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.verticalCenter: parent.verticalCenter

        icon.source: "../../assets/btn_left.svg"
        icon.width: width
        icon.height: height
        icon.color: "transparent"

        background: Rectangle { color: "transparent" }
        onClicked: console.log("Left clicked")
    }

    Button {
        id: btnRight
        width: 34
        height: 40

        padding: 0
        topPadding: 0
        bottomPadding: 0
        leftPadding: 0
        rightPadding: 0

        display: AbstractButton.IconOnly

        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter

        icon.source: "../../assets/btn_right.svg"
        icon.width: width
        icon.height: height
        icon.color: "transparent"

        background: Rectangle { color: "transparent" }
        onClicked: console.log("Right clicked")
    }

    Image {
        id: vignetteBottom
        source: "../../assets/vignette-bottom.svg"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
    }
}
