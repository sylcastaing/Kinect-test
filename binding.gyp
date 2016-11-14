{
    'targets': [
        {
            'target_name': 'kinect',
            'sources': [
                'src/addon.cc',
                'src/kinect.cc'
            ],
            'cflags!': [ '-fno-exceptions' ],
            'cflags_cc!': [ '-fno-exceptions' ]
        }
    ]
}