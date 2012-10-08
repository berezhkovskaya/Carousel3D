1. Когда будет нормальное управление рендер-таргетами, а не хард-код с out_PickMask и out_FragColor,
   то можно слить UpdateForces, UpdateVelocity и UpdatePosition в один шейдер.

   А UpdateForces убрать вообще, т.к. это временная перевычисляемая информация

2. Также неплохо бы

