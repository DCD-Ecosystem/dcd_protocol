const path = require('path');

module.exports = {
    entry: {
        dcdjs_api: './src/dcdjs-api.ts',
        dcdjs_jsonrpc: './src/rpc-web.ts',
        dcdjs_jssig: './src/dcdjs-jssig.ts',
        dcdjs_numeric: './src/dcdjs-numeric.ts',
    },
    devtool: 'inline-source-map',
    mode: 'development',
    module: {
        rules: [
            {
                test: /\.tsx?$/,
                use: {
                    loader: 'ts-loader',
                    options: {
                        configFile: 'tsconfig.web.json'
                    }
                },
                exclude: /node_modules/,
            }
        ]
    },
    resolve: {
        extensions: ['.tsx', '.ts', '.js']
    },
    output: {
        filename: x => x.chunk.name.replace('_', '-') + '-debug.js',
        library: '[name]',
        path: path.resolve(__dirname, 'dist-web', 'debug'),
    }
};
