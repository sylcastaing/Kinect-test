{
    'targets': [
        {
            'target_name': 'kinect',
            'sources': [
                'src/kinect.cc',
                'src/addon.cc'
            ],
            'cflags!': [ '-fno-exceptions' ],
            'cflags_cc!': [ '-fno-exceptions' ]
        }
    ]
}