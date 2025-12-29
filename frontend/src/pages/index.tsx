import { FileCompressionTool } from '../components/file-compression-tool';

export default async function HomePage() {
  return (
    <div className="w-full flex justify-center">
      <title>Compression UI</title>
      <FileCompressionTool />
    </div>
  );
}

export const getConfig = async () => {
  return {
    render: 'static',
  } as const;
};
