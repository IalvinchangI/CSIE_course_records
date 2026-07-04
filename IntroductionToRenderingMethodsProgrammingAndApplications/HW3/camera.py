from panda3d.core import NodePath, Vec3

class Camera():
    # WASD Rotate
    # Perspective View

    def __init__(self, base):
        # Setup Perspective View
        # Init Position and Angle
        # Some setup
        self.base = base
        
        self._camera_root1 = NodePath("CameraRoot1")
        self._camera_root2 = NodePath("CameraRoot2")
        self._camera_base = NodePath("CameraBase")
        self.base.camera.reparentTo(self._camera_base)
        self._camera_base.reparentTo(self._camera_root2)
        self._camera_root2.reparentTo(self._camera_root1)
        
        self._camera_root1.setPos(0, 0, 0)
        self._camera_root2.setPos(0, 0, 0)
        self._camera_base.setPos(0, 5, 1)
        self.base.camera.setPos(0, 0, 0)
        self._camera_root2.lookAt(self._camera_root1)
        self._camera_base.lookAt(self._camera_root2)
        
        self.base.camLens.setFov(60)
        
        self.base.disableMouse()
        self.base.accept("a", self.move, [Vec3(-30, 0, 0)])
        self.base.accept("d", self.move, [Vec3(30, 0, 0)])
        self.base.accept("w", self.move, [Vec3(0, 30, 0)])
        self.base.accept("s", self.move, [Vec3(0, -30, 0)])

    def reparentTo(self, parent):
        self._camera_root1.reparentTo(parent)
        self._camera_root1.setPos(0, 0, 0)
        self._camera_root1.lookAt(parent)

    def move(self, angle: Vec3):
        """
        angle: Vec3(heading, pitch, 0)
        heading: 左右轉動 (繞 Z 軸)
        pitch: 上下轉動 (繞 X 軸)
        """
        current_h = self._camera_root1.getH()
        current_p = self._camera_root2.getP()

        new_h = current_h + angle.getX()
        new_p = current_p + angle.getY()

        self._camera_root1.setH(new_h)
        self._camera_root2.setP(new_p)

    
    @property
    def root(self):
        return self._camera_root1
    
    @property
    def cat(self):
        return self.base.camera
