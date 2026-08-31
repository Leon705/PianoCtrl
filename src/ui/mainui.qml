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
        anchors.leftMargin: 15
        anchors.verticalCenter: parent.verticalCenter

        icon.source: "../../assets/btn_left.svg"
        icon.width: width
        icon.height: height
        icon.color: "transparent"

        background: Rectangle { color: "transparent" }
        onClicked: uiController.switchToAdjacentSampleLibrary(true);
    }

    Button {
        id: btnRight
        x: 281
        width: 34
        height: 40

        padding: 0
        topPadding: 0
        bottomPadding: 0
        leftPadding: 0
        rightPadding: 0

        display: AbstractButton.IconOnly

        anchors.right: parent.right
        anchors.rightMargin: 5
        anchors.verticalCenter: parent.verticalCenter

        icon.source: "../../assets/btn_right.svg"
        icon.width: width
        icon.height: height
        icon.color: "transparent"

        background: Rectangle { color: "transparent" }
        onClicked: uiController.switchToAdjacentSampleLibrary();
    }

    Image {
        id: vignetteBottom
        source: "../../assets/vignette-bottom.svg"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
    }

    Rectangle {
        y: 0
        width: 320
        height: 54
        color: "#13ffffff"
        radius: 2
        anchors.horizontalCenterOffset: 0

        anchors.horizontalCenter: parent.horizontalCenter

        Button {
            id: btnMenu
            width: 24
            height: 24
            visible: true

            padding: 0
            topPadding: 0
            bottomPadding: 0
            leftPadding: 0
            rightPadding: 0

            display: AbstractButton.IconOnly

            anchors.right: parent.right
            anchors.rightMargin:15
            anchors.top: parent.top
            anchors.topMargin: 15

            icon.source: "../../assets/btn_menu.svg"
            icon.width: width
            icon.height: height
            icon.color: "white"
            background: Rectangle { color: "transparent" }
        }

        Label {
            id: labelCurrentSampleLibName
            width: 232
            height: 24

            anchors.right: parent.right
            anchors.rightMargin: 44
            anchors.top: parent.top
            anchors.topMargin: 15
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.pointSize: 10
            clip: false
            font.bold: true
            font.family: "Arial"
            color: "white"
            text: uiController.currentSampleLibraryName
        }
    }
}