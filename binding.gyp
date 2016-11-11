{
    'targets': [
        {
            'target_name': 'kinect',
            'sources': [
                'src/kinect.cc'
            ],
            'cflags!': [ '-fno-exceptions' ],
            'cflags_cc!': [ '-fno-exceptions' ]
        }
    ]
}